import sys

def main() -> None:
    # Golden output matches nxpp to_dot for the same DiGraphInt shape.
    sys.stdout.write(
        'digraph G {\n'
        '  n0 [label="0"];\n'
        '  n1 [label="1"];\n'
        '  n0 -> n1 [weight=2];\n'
        "}\n"
    )

if __name__ == "__main__":
    main()
