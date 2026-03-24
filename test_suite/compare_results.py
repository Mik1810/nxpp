import json
import os
import sys

def load_json(path):
    with open(path, 'r') as f:
        return json.load(f)

def main():
    test_dir = os.path.dirname(os.path.abspath(__file__))
    
    tests = ["bfs", "dijkstra", "cc"]
    all_passed = True
    
    for test in tests:
        expected_path = os.path.join(test_dir, f"{test}_expected.json")
        actual_path = os.path.join(test_dir, f"{test}_actual.json")
        
        if not os.path.exists(expected_path) or not os.path.exists(actual_path):
            print(f"Skipping {test}: missing result files.")
            continue
            
        expected = load_json(expected_path)
        actual = load_json(actual_path)
        
        # Per le componenti connesse, ordiniamo anche l'actual per confrontarle esattamente
        if test == "cc":
            actual = sorted([sorted(c) for c in actual])
        
        if expected == actual:
            print(f"[PASS] {test} matches perfectly.")
        else:
            print(f"[FAIL] {test} mismatch!")
            print(f"  Expected: {expected}")
            print(f"  Actual:   {actual}")
            all_passed = False
            
    if all_passed:
        print("\nAll tests passed (100% match)!")
        sys.exit(0)
    else:
        print("\nSome tests failed.")
        sys.exit(1)

if __name__ == "__main__":
    main()
