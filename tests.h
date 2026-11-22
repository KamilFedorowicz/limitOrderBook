#pragma once

#include <cassert>
#include <iostream>
#include "LimitOrderBook.h"

void testEmptyBook();

void testBasicAdd();

void testMultipleOrdersAtPrice();

void duplicateID();
void modifyTest();

void cancelTest();

void cancelUnknownID();

void runAllTests();
