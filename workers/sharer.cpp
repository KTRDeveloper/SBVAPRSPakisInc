#include "../light.hpp"
#include "basesolver.hpp"
#include "sharer.hpp"
#include "clause.hpp"
#include <unistd.h>
#include <boost/thread/thread.hpp>
#include <chrono>

void * share_worker(void *arg) {
    int nums = 0;
    sharer * sq = (sharer *)arg;
    auto clk_st = std::chrono::high_resolution_clock::now();
    double share_time = 0;
    while (true) {
        ++nums;
        usleep(sq->share_intv);
        auto clk_now = std::chrono::high_resolution_clock::now();
        int solve_time = std::chrono::duration_cast<std::chrono::milliseconds>(clk_now - clk_st).count();
        if (terminated) break;
        for (int i = 0; i < sq->producers.size(); i++) {
            sq->cls.clear();
            sq->producers[i]->export_clauses_to(sq->cls);
            int number = sq->cls.size();
            int percent = sq->sort_clauses(i);
            if (percent < 75) {
                sq->producers[i]->broaden_export_limit();
            }
            else if (percent > 98) {
                sq->producers[i]->restrict_export_limit();
            }
          
            for (int j = 0; j < sq->consumers.size(); j++) {
                if (sq->producers[i]->id == sq->consumers[j]->id) continue;
                for (int k = 0; k < sq->cls.size(); k++)
                    sq->cls[k]->increase_refs(1);
                sq->consumers[j]->import_clauses_from(sq->cls);
            }
            for (int k = 0; k < sq->cls.size(); k++) {
                sq->cls[k]->free_clause();
            }
        }
        auto clk_ed = std::chrono::high_resolution_clock::now();
        share_time += 0.001 * std::chrono::duration_cast<std::chrono::milliseconds>(clk_ed - clk_now).count();
    }
    return NULL;
}
 
int sharer::import_clauses(int id) {
    int current_period = producers[id]->get_period() - 1, import_period = current_period - margin;
    if (import_period < 0) return 0;
    basesolver *t = producers[id];
    for (int i = 0; i < producers.size(); i++) {
        if (i == id) continue;
        basesolver *s = producers[i];
        
        boost::mutex::scoped_lock lock(s->mtx);
        while (s->period <= import_period && s->terminate_period > import_period && !s->terminated) {
            s->cond.wait(lock);
        }
        
        if (s->terminated) return -1;
        if (s->terminate_period <= import_period) return -1; 
        
        period_clauses *pc = s->pq.find(import_period);
        
        t->import_clauses_from(pc->cls);
    }
    t->unfree.push(import_period);
    return 1;
}

int sharer::sort_clauses(int x) {
    for (int i = 0; i < cls.size(); i++) {
        int sz = cls[i]->size;
        while (sz > bucket[x].size()) bucket[x].push();
        if (sz * (bucket[x][sz - 1].size() + 1) <= share_lits) 
            bucket[x][sz - 1].push(cls[i]);
    }
    cls.clear();
    int space = share_lits;
    for (int i = 0; i < bucket[x].size(); i++) {
        int clause_num = space / (i + 1);
        if (!clause_num) break;
        if (clause_num >= bucket[x][i].size()) {
            space -= bucket[x][i].size() * (i + 1);
            for (int j = 0; j < bucket[x][i].size(); j++)
                cls.push(bucket[x][i][j]);
            bucket[x][i].clear();
        }
        else {
            space -= clause_num * (i + 1);
            for (int j = 0; j < clause_num; j++) {
                cls.push(bucket[x][i].last());
                bucket[x][i].pop();
            }
        }
    }
    return (share_lits - space) * 100 / share_lits;
}

void light::share() {
    if (OPT(DCE)) {
        sharer* s = new sharer(0, OPT(share_intv), OPT(share_lits), OPT(DCE));
        s->margin = OPT(margin);
        for (int j = 0; j < OPT(nThreads); j++) {
            s->producers.push(workers[j]);
            workers[j]->in_sharer = s;
        }
        sharers.push(s);
    }
    else {
        int sharers_number = 1;
        for (int i = 0; i < sharers_number; i++) {
            sharer* s = new sharer(i, OPT(share_intv), OPT(share_lits), OPT(DCE));
            for (int j = 0; j < OPT(nThreads); j++) {
                s->producers.push(workers[j]);
                s->consumers.push(workers[j]);
                workers[j]->in_sharer = s;
            }
            sharers.push(s);
        }
        
        pthread_t *ptr = new pthread_t[sharers_number];
        for (int i = 0; i < sharers_number; i++) {
            pthread_create(&ptr[i], NULL, share_worker, sharers[i]);
        }
    }   
}
