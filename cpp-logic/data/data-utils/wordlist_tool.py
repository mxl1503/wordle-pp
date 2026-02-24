#!/usr/bin/env python3
"""Utilities for maintaining wordlist data files."""

from __future__ import annotations

import argparse
from pathlib import Path


def sort_words(input_file: Path, output_file: Path) -> None:
    words = [line.strip() for line in input_file.read_text(encoding="utf-8").splitlines() if line.strip()]
    words.sort()
    output_file.write_text("\n".join(words) + ("\n" if words else ""), encoding="utf-8")
    print(f"Sorted {len(words)} words into {output_file}")


def generate_permutations(output_file: Path, length: int, alphabet: str) -> None:
    if length <= 0:
        raise ValueError("--length must be > 0")
    if not alphabet:
        raise ValueError("--alphabet must not be empty")

    lines: list[str] = []

    def build(prefix: str, remaining: int) -> None:
        if remaining == 0:
            lines.append(prefix)
            return
        for char in alphabet:
            build(prefix + char, remaining - 1)

    build("", length)
    output_file.write_text("\n".join(lines) + ("\n" if lines else ""), encoding="utf-8")
    print(f"Generated {len(lines)} permutations to {output_file}")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Wordlist data utility CLI")
    subparsers = parser.add_subparsers(dest="command", required=True)

    sort_cmd = subparsers.add_parser("sort", help="Sort newline-delimited words")
    sort_cmd.add_argument("--in", dest="input_file", type=Path, required=True, help="Input wordlist file")
    sort_cmd.add_argument("--out", dest="output_file", type=Path, required=True, help="Output wordlist file")

    perms_cmd = subparsers.add_parser("perms", help="Generate fixed-length permutations")
    perms_cmd.add_argument("--out", dest="output_file", type=Path, required=True, help="Output file")
    perms_cmd.add_argument("--length", type=int, default=5, help="Permutation length (default: 5)")
    perms_cmd.add_argument("--alphabet", type=str, default="BGY", help="Alphabet to permute (default: BGY)")

    return parser.parse_args()


def main() -> int:
    args = parse_args()
    if args.command == "sort":
        sort_words(args.input_file, args.output_file)
        return 0
    if args.command == "perms":
        generate_permutations(args.output_file, args.length, args.alphabet)
        return 0
    raise RuntimeError(f"Unsupported command: {args.command}")


if __name__ == "__main__":
    raise SystemExit(main())
