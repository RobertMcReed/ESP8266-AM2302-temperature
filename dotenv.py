# read a .env file and add all variables to the CPPDEFINES to be accessible in the script

Import("env")

build_vars = []

with open('.env', 'r') as f:
    lines = f.readlines()

    for line in lines:
        if line[0] == '#':
            continue
        
        key, val = line.strip().split('=')
        string_val = "\\\"" + val.replace("\\", "\\\\") + "\\\""
        build_vars.append(('__{}__'.format(key), string_val))

env.Append(CPPDEFINES=build_vars)
