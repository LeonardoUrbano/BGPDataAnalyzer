#ifndef COMMON_H_1234
#define COMMON_H_1234
#include <pthread.h>
#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
using namespace std;

// Number of repositories
#define N_REPOSITORY 3
// Number of route collectors
#define N_RC_RV 18
#define N_RC_RIS 13
#define N_RC_ISO 3
// Burst time in minutes
#define first_bgpupd_since 15
// Local directories
#define BaseDirRV "../REP/RV/"
#define BaseDirRis "../REP/RIS/"
#define BaseDirIso "../REP/ISO/"



// Global variables shared among the threads
extern pthread_mutex_t count_mutex;
extern pthread_cond_t  condition_var;
extern int threadCount;



// Route collectors
const string RVIEWS []= {
"",
"route-views3/",
"route-views4/",
"route-views6/",
"route-views.eqix/",
"route-views.isc/",
"route-views.kixp/",
"route-views.jinx/",
"route-views.linx/",
"route-views.nwax/",
"route-views.telxatl/",
"route-views.wide/",
"route-views.sydney/",
"route-views.saopaulo/",
"route-views.sg/",
"route-views.perth/",
"route-views.sfmix/",
"route-views.soxrs/"
};

const string RIS []= {
"rrc00/",
"rrc01/",
"rrc03/",
"rrc04/",
"rrc05/",
"rrc06/",
"rrc07/",
"rrc10/",
"rrc11/",
"rrc12/",
"rrc13/",
"rrc14/",
"rrc15/"
};

const string ISO []= {
"Alderaan/",
"Dagobah/",
"Naboo/"
};
#endif
