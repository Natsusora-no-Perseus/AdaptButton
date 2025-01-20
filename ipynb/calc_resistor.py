# A simple Python script to calculate the required resistor values for the op amps.
# First solve([1+r2*(r3+r4)/(r3*r4)==15, r4/(r3+r4)*5==2.45],(r2,r3,r4)), we find
#   that R2:R3:R4 = 1: 50/343 : 50/357

### Now we can try to find the resistor values ...
# Define the ratio
ratio_R2_to_R3 = 50/343
ratio_R2_to_R4 = 50/357

# Common resistor series (E24: 24 values per decade)
# e24_series = [
#     1.0, 1.1, 1.2, 1.3, 1.5, 1.6, 1.8, 2.0, 2.2, 2.4, 2.7, 3.0,
#     3.3, 3.6, 3.9, 4.3, 4.7, 5.1, 5.6, 6.2, 6.8, 7.5, 8.2, 9.1
# ]

# What I have at hand:
e24_full_series = [
        10, 22, 47, 100, 150, 200, 220, 270, 330, 470, 680,
        1000, 2000, 2200, 3300, 4700, 5100, 6800,
        10000, 20000, 47000, 51000, 68000, 100000, 220000, 300000, 470000, 510000,
        680000, 1e6
        ]

# Expand the series to cover multiple decades (10^0 to 10^6)
# decades = [10**i for i in range(-1, 7)]  # Decades from 0.1 to 1,000,000 ohms
# e24_full_series = [r * d for d in decades for r in e24_series]

# Function to find the closest resistor value from the E24 series
def find_closest_resistor(value, series):
    return min(series, key=lambda x: abs(x - value))

def get_resistor_notation(value):
    if value >= 1e6:
        ret = f"{value / 1e6:.1f}".rstrip("0").rstrip(".") + "M"
    elif value >= 1e3:
        ret = f"{value / 1e3:.1f}".rstrip("0").rstrip(".") + "K"
    else:
        ret = f"{value:.1f}".rstrip("0").rstrip(".") + "R"

    return ret

# Try base values for R2 and compute R3, R4
results = []

for R2 in e24_full_series:
    R3 = ratio_R2_to_R3 * R2
    R4 = ratio_R2_to_R4 * R2

    # Find closest standard values
    R3_closest = find_closest_resistor(R3, e24_full_series)
    R4_closest = find_closest_resistor(R4, e24_full_series)

    # Calculate the resulting ratios
    ratio_R3 = R3_closest / R2
    ratio_R4 = R4_closest / R2

    # Store results if ratios are close to the desired values
    if abs(ratio_R3 - ratio_R2_to_R3) < 0.005 and \
            abs(ratio_R4 - ratio_R2_to_R4) < 0.005:
        results.append((R2, R3_closest, R4_closest, ratio_R3, ratio_R4))
        print("Found possible pair: R2={}, R3={}, R4={}".format(
            get_resistor_notation(R2),
            get_resistor_notation(R3_closest),
            get_resistor_notation(R4_closest)))

