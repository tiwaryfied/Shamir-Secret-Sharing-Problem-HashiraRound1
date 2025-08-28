# Shamir's Secret Sharing Solution

**Catalog Placements Assignment**

## Problem
Find the constant term of a polynomial using Shamir's Secret Sharing algorithm from encoded points in different bases.

## Approach
- **Base Conversion**: Decode y-values from various number bases (2-36)
- **Lagrange Interpolation**: Reconstruct polynomial and find f(0)
- Uses exactly k points to solve for the secret constant

## Usage
```bash
g++ -o solution solution.cpp
./solution
```

## Results
- Test Case 1: **3**
- Test Case 2: **79637716543053734**

## Implementation
- Language: C++
- No external dependencies
- Handles large numbers with `long long`
- Time complexity: O(k²)

---

Clean implementation of the mathematical algorithm with proper base conversion and interpolation.
