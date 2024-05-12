#include "light.hpp"
#include "workers/basekissat.hpp"
#include "workers/sharer.hpp"
#include <unistd.h>
#include <chrono>
#include <algorithm>
#include <mutex>
auto clk_st = std::chrono::high_resolution_clock::now();
char* worker_sign = "";

std::atomic<int> terminated;
int result = 0;
int winner_conf;
vec<int> model;

void * read_worker(void *arg) {
   basesolver * sq = (basesolver *)arg;
    if (worker_sign == "") 
      sq->parse_from_PAR(sq->controller->pre);
   else 
      sq->parse_from_CNF(worker_sign);
   return NULL;
}

void * solve_worker(void *arg) {
   basesolver * sq = (basesolver *)arg;
   while (!terminated) {
        int res = sq->solve();
        if (sq->controller->opt->DCE) {
            if (res) {
                terminated = 1;
                result = res; 
                sq->internal_terminate();
                sq->controller->update_winner(sq->id, sq->period);
                if (res == 10) sq->get_model(sq->model);
            }
        }
        else {
            if (res && !terminated) {
                terminated = 1;
                sq->controller->terminate_workers();
                result = res;
                sq->controller->update_winner(sq->id, 0);
                winner_conf = sq->get_conflicts();
                if (res == 10) sq->get_model(sq->model);
            }
        }
   }
   return NULL;
}

void light::init_workers() {
    terminated = 0;
    for (int i = 0; i < OPT(nThreads); i++) {
        basekissat* kissat = new basekissat(i, this);
        workers.push(kissat);
    }
}

void light::diversity_workers() {
    for (int i = 0; i < OPT(nThreads); i++) {
        if (OPT(shuffle)) {
            if (i) workers[i]->configure("order_reset", i);
        }
        if (OPT(pakis)) {

            int configId = i%24;
            switch(configId){ 
                case 0 : workers[i]->set_configuration("default");
                    break;
                case 1 : 
                workers[i]->set_configuration("sat");
                    break;
                case 2 : 
                    workers[i]->set_configuration("unsat");
                    break;  
                case 3 : 
                    workers[i]->set_configuration("default");
                    workers[i]->configure("chrono", false);
                    break;    
                case 4 :
                    workers[i]->set_configuration("default");
                    workers[i]->configure("chrono", false);
                    workers[i]->configure("phase", false);
                    
                    break;
                case 5 : 
                    workers[i]->set_configuration("default");
                    workers[i]->configure("phase", false);

                    break;
                case 6 : 
                    workers[i]->set_configuration("default");
                    workers[i]->configure("stable", 2);
                    workers[i]->configure("chrono", false);
                    break;
                case 7 : 
                    workers[i]->set_configuration("default");
                    workers[i]->configure("target", 0);

                    break;
                case 8 : 
                    workers[i]->set_configuration("default");
                    workers[i]->configure("target", 0);
                    workers[i]->configure("chrono", false);
                    workers[i]->configure("phase", false);

                    break;
                case 9 : 
                    workers[i]->set_configuration("default");
                    workers[i]->configure("target", 1);
                    workers[i]->configure("walkinitially", true);
                    workers[i]->configure("chrono", true);

                    break;
                case 10 : 
                    workers[i]->set_configuration("default");
                    workers[i]->configure("walkinitially", true);

                    break;
                case 11 : 
                    workers[i]->set_configuration("sat");
                    workers[i]->configure("chrono", false);
                    
                    break;
                case 12 : 
                    workers[i]->set_configuration("sat");
                    workers[i]->configure("chrono", false);
                    workers[i]->configure("phase", false);

                    break;
                case 13 : 
                    workers[i]->set_configuration("sat");
                    workers[i]->configure("chrono", false);
                    workers[i]->configure("phase", false);
                    workers[i]->configure("tier1", 3);

                    break;
                case 14 : 
                    workers[i]->set_configuration("sat");
                    workers[i]->configure("chrono", false);
                    workers[i]->configure("tier1", 3);

                    break;
                case 15 : 
                    workers[i]->set_configuration("sat");
                    workers[i]->configure("phase", false);

                    break;
                case 16 : 
                    workers[i]->set_configuration("sat");
                    workers[i]->configure("stable", 2);
                    workers[i]->configure("chrono", false);
                    break;
                case 17 : 
                    workers[i]->set_configuration("default");
                    workers[i]->configure("target", 0);
                    break;
                case 18 : 
                    workers[i]->set_configuration("default");
                    workers[i]->configure("target", 0);
                    workers[i]->configure("chrono", false);
                    workers[i]->configure("phase", false);
                    break;
                case 19 : 
                    workers[i]->set_configuration("default");
                    workers[i]->configure("target", 0);
                    workers[i]->configure("phase", false);
                    
                    break;
                case 20 : 
                    workers[i]->set_configuration("unsat");
                    workers[i]->configure("target", 0);
                    workers[i]->configure("tier1", 3);
                    workers[i]->configure("psids", 1);
                    break;
                case 21 : 
                    workers[i]->set_configuration("unsat");
                    workers[i]->configure("tier1", 3);
                    workers[i]->configure("psids", 1);
                    break;
                case 22 : 
                    workers[i]->set_configuration("unsat");
                    workers[i]->configure("walkinitially", true);
                    workers[i]->configure("psids", 1);
                    break;
                case 23 : 
                    workers[i]->set_configuration("unsat");
                    workers[i]->configure("chrono", false);
                    workers[i]->configure("psids", 1);
                    break;
                default:
                    break;
            }
            
        }
    }
}

void light::terminate_workers() {
    for (int i = 0; i < OPT(nThreads); i++) {
        if (OPT(clause_sharing) == 1 && OPT(DCE) == 1) 
            workers[i]->external_terminate();
        else 
            workers[i]->terminate();
    }
    for (int i = 0; i < sharers.size(); i++) {
        sharers[i]->set_terminated();
    }
}

void light::parse_input() {
    pthread_t *ptr = new pthread_t[OPT(nThreads)];
    for (int i = 0; i < OPT(nThreads); i++) {
      pthread_create(&ptr[i], NULL, read_worker, workers[i]);
   }   
   for (int i = 0; i < OPT(nThreads); i++) {
      pthread_join(ptr[i], NULL);
   }    
   delete []ptr;
}

int light::solve() {
    pthread_t *ptr = new pthread_t[OPT(nThreads)];
    for (int i = 0; i < OPT(nThreads); i++) {
      pthread_create(&ptr[i], NULL, solve_worker, workers[i]);
    }

    while (!terminated) {
        usleep(100000);        
        auto clk_now = std::chrono::high_resolution_clock::now();
        int solve_time = std::chrono::duration_cast<std::chrono::seconds>(clk_now - clk_st).count();
        if (solve_time >= OPT(cutoff)) {
            terminated = 1;
            terminate_workers();
        }
    }

    for (int i = 0; i < OPT(nThreads); i++) {
        pthread_join(ptr[i], NULL);
    }
    
    if (result == 10)
        workers[winner_id]->model.copyTo(model);
    delete []ptr;
    return result;
}

int light::run() {
    init_workers();
    diversity_workers();
    if (OPT(preprocessor)) {
        pre = new preprocess();
        int res = pre->do_preprocess(filename);
        if (res == 20) return 20;
        else if (res == 10) {
            for (int i = 1; i <= pre->vars; i++) {
                model.push(pre->model[i]);
            }
            return 10;
        }
    }
    else worker_sign = filename;
    parse_input();
    if (OPT(clause_sharing)) share();
    int res = solve();
    if (res == 10 && OPT(preprocessor)) {
        for (int i = 1; i <= pre->orivars; i++)
            if (pre->mapto[i]) pre->mapval[i] = (model[abs(pre->mapto[i])-1] > 0 ? 1 : -1) * (pre->mapto[i] > 0 ? 1 : -1);
        pre->get_complete_model();
        model.clear();
        for (int i = 1; i <= pre->orivars; i++) {
            model.push(i * pre->mapval[i]);
        }    
    }
    return res;
}

void print_model(vec<int> &model) {
    printf("v");
    for (int i = 0; i < model.size(); i++) {
        printf(" %d", model[i]);
    }
    puts(" 0");
}

int solve(int argc, char **argv) {
    light* S = new light();
    S->arg_parse(argc, argv);
    int res = S->run();
    if (res == 10) {
        printf("s SATISFIABLE\n");
        print_model(model);
    }
    else if (res == 20) {
        printf("s UNSATISFIABLE\n");
    }
    else {
        printf("s UNKNOWN\n");
    }
    delete(S);
    return res;
}
