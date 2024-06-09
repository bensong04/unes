/**
 * @file
 * @brief
 *
 * @author Benedict Song <benedict04song@gmail.com>
 */
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#include "cpu/ucpu.h"
#include "cpu/uerrno.h"
#include "memory/umem.h"
#include "memory/urom.h"

#define NESTEST_SZ 0xFFFFu
#define NES_HEADER_SZ 16

clk_t global_clock;
uint64_t instrs_execed;
uerrno_t UERRNO;

bool stopped = false;

void alarm_handler(int signal) {
  printf("%llu instructions executed in %llu clock cycles\n", instrs_execed,
         global_clock);
  stopped = true;
}

typedef struct cpu_state {
  uaddr_t PC;
  uregr_t A;
  uregr_t X;
  uregr_t Y;
  uregr_t S;
  ustat_t status;
} cpu_state_t;

void dump_cpu_state(FILE *out, cpu_state_t stat) {
  fprintf(out,
          "PC: %" PRIu16 " A: %" PRIu8 " X: %" PRIu8
          " "
          "Y: %" PRIu8 " S: %" PRIu8 " status: %" PRIu8 "\n",
          stat.PC, stat.A, stat.X, stat.Y, stat.S, stat.status);
}

bool check_stats_equal(ucpu_t cpu, cpu_state_t state) {
  return (cpu.PC == state.PC) && (cpu.A == state.A) && (cpu.X == state.X) &&
         (cpu.Y == state.Y) && (cpu.S == state.S) &&
         (cpu.status == state.status);
}

void preset_bus_byte(bus_t *bus, uaddr_t which, byte_t what) {
  bus->cpu_ram[which] = what;
}

void preset_cpu_state(ucpu_t *cpu, cpu_state_t state) {
  cpu->PC = state.PC;
  cpu->A = state.A;
  cpu->X = state.X;
  cpu->Y = state.Y;
  cpu->S = state.S;
  cpu->status = state.status;
}

int main(int argc, char **argv) {
  int nesfd = open("eval/execs/official.nes", O_RDWR);
  byte_t *executable = mmap(NULL, NESTEST_SZ + NES_HEADER_SZ,
                            PROT_READ | PROT_WRITE, MAP_PRIVATE, nesfd, 0) +
                       NES_HEADER_SZ;
  close(nesfd);
  if (executable == (void *)-1) {
    perror(strerror(errno));
    exit(1);
  }

  uaddr_t exe_start = 0xE000;

  // setup everything
  ucpu_t cpu;
  init_cpu(&cpu);

  bus_t bus = new_bus();
  bus.cartridge = executable;

  link_device(&cpu.buslink, &bus);  // this is fine I think?

  cpu_state_t stat = {0};
  stat.PC = exe_start;
  stat.S = 0xFD;
  stat.status = 0x24;

  preset_cpu_state(&cpu, stat);
  dump_cpu(stdout, &cpu);

  signal(SIGALRM, &alarm_handler);

  alarm(10);  // sample for 10 seconds
  while (!stopped) {
    int res = step(&cpu);
    if (cpu.cycs_left == 0) {
      instrs_execed++;
    }
    global_clock++;
  }
  dump_cpu(stdout, &cpu);
  exit(0);
}
