

def version():
    result = []
    with open("version") as file:
        for line in file.readlines():
            parts = line.replace(';','').split('=')
            if 2 != len(parts):
                continue

            result.append(parts[1])

    return result


