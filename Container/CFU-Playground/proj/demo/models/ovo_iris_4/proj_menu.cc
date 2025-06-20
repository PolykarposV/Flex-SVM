/*
 * Copyright 2021 The CFU-Playground Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "proj_menu.h"

#include <stdio.h>

#include "cfu.h"
#include "menu.h"

namespace {

// Template Fn

void do_hello_world(void) { puts("Hello, World!!!\n"); }

// Test template instruction
void do_grid_cfu_op0(void) {
  puts("\nExercise CFU Op0\n");
  printf("a   b-->");
  for (int b = 0; b < 6; b++) {
    printf("%8d", b);
  }
  puts("\n-------------------------------------------------------");
  for (int a = 0; a < 6; a++) {
    printf("%-8d", a);
    for (int b = 0; b < 6; b++) {
      int cfu = cfu_op0(0, a, b);
      printf("%8d", cfu);
    }
    puts("");
  }
}

// Test template instruction
void do_grid_cfu_op1(void) {
  puts("\nExercise CFU Op0\n");
  printf("a   b-->");
  for (int b = -3; b < 4; b++) {
    printf("%8d", b);
  }
  puts("\n-------------------------------------------------------");
  for (int a = -3; a < 4; a++) {
    printf("%-8d", a);
    for (int b = -3; b < 4; b++) {
      int cfu = cfu_op1(0, a, b);
      printf("%8d", cfu);
    }
    puts("");
  }
}

// Test template instruction
void do_exercise_cfu_op0(void) {
  puts("\nExercise CFU Op0\n");
  int count = 0;
  for (int32_t a = -0x71234567; a < 0x68000000; a += 0x10012345) {
    for (int32_t b = -0x7edcba98; b < 0x68000000; b += 0x10770077) {
      int32_t cfu = cfu_op0(0, a, b);
      int32_t exp = a*b>>10;
      int32_t ab = a*b;
      int32_t cfuab = cfu_op1(1, a, b);
      printf("a: %08x b:%08x cfu=%08x exp=%08x  ab=%08x\n", (unsigned)a, (unsigned)b, (unsigned)cfu, (unsigned)exp, (unsigned)ab);
      if (cfu != exp) {
        printf("\n***FAIL\n");
        printf("\nexpect %08x  a*b=%08x  cfu:a*b=%08x\n", (unsigned)exp, (unsigned)ab, (unsigned)cfuab);
        return;
      }
      count++;
    }
  }
  printf("Performed %d comparisons", count);
}

// Test template instruction
void do_exercise_cfu_op1(void) {
  puts("\nExercise CFU Op1\n");
  int count = 0;
  for (int a = -0x71234567; a < 0x68000000; a += 0x10012345) {
    for (int b = -0x7edcba98; b < 0x68000000; b += 0x10770077) {
      int cfu = cfu_op1(0, a, b);
      printf("a: %08x b:%08x cfu=%08x\n", (unsigned)a, (unsigned)b, (unsigned)cfu);
      if (cfu != a*b) {
        printf("\n***FAIL\n");
        printf("\nexpect %08x\n", (unsigned)(a*b));
        return;
      }
      count++;
    }
  }
  printf("Performed %d comparisons", count);
}

struct Menu MENU = {
    "Project Menu",
    "project",
    {
        MENU_ITEM('0', "exercise cfu op0", do_exercise_cfu_op0),
        MENU_ITEM('1', "exercise cfu op1", do_exercise_cfu_op1),
        MENU_ITEM('g', "grid cfu op0", do_grid_cfu_op0),
        MENU_ITEM('G', "grid cfu op1", do_grid_cfu_op1),
        MENU_ITEM('h', "say Hello", do_hello_world),
        MENU_END,
    },
};

};  // anonymous namespace

extern "C" void do_proj_menu() { menu_run(&MENU); }
