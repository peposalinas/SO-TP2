#ifndef SHELLCOMMANDS_H
#define SHELLCOMMANDS_H

#include "userLib.h"
#include "test_util.h"
#include "definitions.h"
#include "syscaller.h"
#include "libc.h"
#include "syscaller.h"

void setIOPipes(int iPipe, int oPipe);
void setWaitPID(uint64_t pid);
int getWaitPID();
void listAllProcesses();
void clearCmd(int argc, char *argv[]);
void div0(int argc, char *argv[]);
void fontSmall(int argc, char *argv[]);
void fontBig(int argc, char *argv[]);
void help(int argc, char *argv[]);
int helpProc(int argc, char *argv[]);
void invalidOpCode(int argc, char *argv[]);
void createTestSync(int argc, char *argv[]);
void createTestMemInfo(int argc, char *argv[]);
void createTestProcesses(int argc, char *argv[]);
void createTestPrio(int argc, char *argv[]);
void createTestMem(int argc, char *argv[]);
void loop(int argc, char *argv[]);
int loopPrinter(int argc, char *argv[]);
void kill(int argc, char *argv[]);
void nice(int argc, char *argv[]);
void block(int argc, char *argv[]);
int memStatusProc(int argc, char *argv[]);
void memStatusPrinter(int argc, char *argv[]);
void catCmd(int argc, char *argv[]);
int catProc(int argc, char *argv[]);
void wc(int argc, char *argv[]);
int wcProc(int argc, char *argv[]);
void filter(int argc, char *argv[]);
int filterVowelProc(int argc, char *argv[]);
void philo(int argc, char *argv[]);
void philo2(int argc, char *argv[]);
int philosophersRun(int argc, char *argv[]);
int runPhilosophers(int argc, char **argv);
int listAllProcessesProc(int argc, char *argv[]);

#endif