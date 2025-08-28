# Shamir's Secret Sharing Solution

**Catalog Placements Assignment**

## Problem
Find the constant term of a polynomial using Shamir's Secret Sharing algorithm from encoded points in different bases.

## Approach
- **Base Conversion**: Decode y-values from various number bases (2-36)
- **Lagrange Interpolation**: Reconstruct polynomial and find f(0)
- Uses exactly k points to solve for the secret constant

## Core Algorithm: Lagrange Interpolation

f(0) = Σ(yi × Li(0)) where Li(0) = Π((0-xj)/(xi-xj))

## Usage
```bash
g++ -o testcase1 testcase1.cpp
./testcase1

g++ -o testcase2 testcase2.cpp
./testcase2
```

## Results
- Test Case 1: **3**
- Test Case 2: **79637716543053734**

## Implementation
- Language: C++
- Separate files for each test case
- No external dependencies
- Handles large numbers with `long long`
- Time complexity: O(k²)

## Files
- `testcase1.cpp` - Solution for first test case
- `testcase2.cpp` - Solution for second test case
- `testcase1.json` - Input data for test case 1
- `testcase2.json` - Input data for test case 2

---

Clean implementation of the mathematical algorithm with proper base conversion and interpolation.
