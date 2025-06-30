#!/usr/bin/env python3
"""
Complete pipeline for Equation 172:
1. Load F, G, c from inputs
2. Symbolically construct KKT system
3. Export .ms file
4. Run msolve on it
5. Save result to ms_outputs/
"""

import os
import json
import subprocess
from sympy import Matrix, symbols, simplify, expand

# ----------- Load parameters -----------
params_path = "/app/inputs/eq172_params.json"
with open(params_path, "r") as f:
    params = json.load(f)

F = Matrix(params["F"])
G = Matrix(params["G"])
c_val = params["c"]

# ----------- Define variables -----------
x1, x2, y1, y2 = symbols("x1 x2 y1 y2")
lam1, lam2 = symbols("lam1 lam2")
x = Matrix([x1, x2])
y = Matrix([y1, y2])

# ----------- Objective + Lagrangian -----------
expr = (F + y1 * G) * x + Matrix([0, c_val * y2])
f0 = expand(expr.dot(expr))
L = f0 + lam1 * (x.dot(x) - 1) + lam2 * (y.dot(y) - 1)

grad_x = [simplify(L.diff(v)) for v in [x1, x2]]
grad_y = [simplify(L.diff(v)) for v in [y1, y2]]
constraints = [x1**2 + x2**2 - 1, y1**2 + y2**2 - 1]
equations = grad_x + grad_y + constraints

# ----------- Export to .ms file -----------
os.makedirs("/app/ms_test_inputs", exist_ok=True)
ms_path = "/app/ms_test_inputs/eq172_input.ms"

with open(ms_path, "w") as f:
    f.write("x1,x2,y1,y2,lam1,lam2\n")
    f.write("0\n")
    for i, eq in enumerate(equations):
        f.write(str(eq).replace("**", "^"))
        f.write(",\n" if i < len(equations) - 1 else "\n")

print(f"[✓] .ms file written to {ms_path}")

# ----------- Call msolve -----------
os.makedirs("/app/ms_outputs", exist_ok=True)
ms_output_path = "/app/ms_outputs/eq172_input.out"
subprocess.run([
    "msolve",
    "-f", ms_path,
    "-o", ms_output_path
], check=True)

print(f"[✓] msolve output saved to {ms_output_path}")
