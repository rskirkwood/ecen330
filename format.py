#!/usr/bin/python3

import pathlib
import re
import subprocess
import argparse
import sys

repo_root_dir = pathlib.Path(__file__).parent.absolute()

dirs_to_format = ["lab", "my_libs", "lasertag"]
extensions_to_format = [".c", ".h"]

dirs_to_exclude = [
    "build/",
    "platforms/hw/",
    "platforms/zybo/xil_arm_toolchain/bsp",
    "lasertag/sounds",
]


def main():

    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--check",
        action="store_true",
        help="Check that all files are formatted, and error otherwise.",
    )
    parser.add_argument(
        "--all", action="store_true", help="Match against all files, not just the student files."
    )
    args = parser.parse_args()

    dirs_matched = []

    # Loop through all items in tree, and find directories that match any regex in 'dirs_to_format'
    for d in repo_root_dir.glob("**"):
        if not d.is_dir():
            continue

        d_rel = str(d.relative_to(repo_root_dir))

        exclude = False
        for dir_re in dirs_to_exclude:
            if re.match(dir_re, d_rel):
                exclude = True
                break
        if exclude:
            continue

        # If --all option is set, match against everything except the black-list (dirs_to_exclude_from_all),
        # otherwise match against the white list (dirs_to_format)
        if args.all:
            dirs_matched.append(d)
        else:
            for dir_re in dirs_to_format:
                if re.match(dir_re, d_rel):
                    dirs_matched.append(d)
                    break

    # Find all files in matched directories with a matching extension and format them
    num_formatted = 0
    for d in dirs_matched:
        for ext in extensions_to_format:
            for f in d.glob("*" + ext):

                # The first command performs a diff on the original file and the
                # clang-format output to see if clang-format will change anything
                diff_cmd = [
                    "/bin/bash",
                    "-c",
                    "diff -u <(cat " + str(f) + ") <(clang-format " + str(f) + ")",
                ]
                p = subprocess.Popen(diff_cmd, stdout=subprocess.PIPE)
                stdout = p.communicate()[0]

                # If there is any difference, format, or error if the --check option is set
                if len(stdout):
                    if args.check:
                        print("Error:", str(f), "needs formatting.")
                        sys.exit(1)
                    print("Formatting", f.relative_to(repo_root_dir))

                    cmd = ["clang-format", "-i", "-style=LLVM", f]
                    subprocess.run(cmd)

                    num_formatted += 1

    if num_formatted == 0:
        print("Nothing needs formatting.")


if __name__ == "__main__":
    main()
