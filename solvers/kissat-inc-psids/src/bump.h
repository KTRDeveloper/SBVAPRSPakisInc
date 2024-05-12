#ifndef _bump_h_INCLUDED
#define _bump_h_INCLUDED

struct kissat;

void kissat_bump_variables (struct kissat *);
void kissat_bump_chb (struct kissat *, unsigned idx, double multiplier);
void kissat_init_shuffle (struct kissat *, int maxvar);
void kissat_decay_chb (struct kissat *);
void kissat_shuffle_score(struct kissat *);
void kissat_update_conflicted_chb (struct kissat *);
void pol_dec_act(struct kissat *);
void bump_pol_sc (struct kissat *, unsigned);
#define MAX_SCORE 1e150

#endif
