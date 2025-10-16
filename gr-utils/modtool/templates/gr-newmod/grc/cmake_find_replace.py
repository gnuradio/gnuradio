import sys


def find_replace(file_path, old_string, new_string):
    with open(file_path, 'r') as file:
        content = file.read()

    content = content.replace(old_string, new_string)

    with open(file_path, 'w') as file:
        file.write(content)


if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: cmake_find_replace.py <file_path> <old_string> <new_string>")
        sys.exit(1)

    find_replace(sys.argv[1], sys.argv[2], sys.argv[3])
