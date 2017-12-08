#!/usr/bin/env python

#===========================================================================
#
# create and install pkg-config file
#
#===========================================================================

import os
import sys
import glob
import re

global thisScriptName

# write to lrose-config.flags 
def write_common_flags(lib_info, app_info):
    install_dir = os.environ['LROSE_INSTALL_DIR']
    core_dir = os.environ['LROSE_CORE_DIR']

    filename = os.path.join(install_dir, 'bin', 'lrose-config.flags')
    f = open(filename, 'a')
    # get the all_<flags> for the apps and the libs and merge them
    libs_flags = lib_info['libs']
    apps_flags = app_info['apps']
    for key in ['includes', 'libs', 'ldflags', 'cflags']:
#     info['all'] =  {"includes":all_includes, "libs":all_libs, "ldflags":all_ldflags, "cflags":all_cflags}
        combined_set = libs_flags[key]
        all_together = combined_set.union(apps_flags[key])

        result = key + '='
        if (len(all_together) > 0):
            result += formatted(all_together)
        f.write(result + "\n")
    f.close()


# write to lrose-config.flags 
def write_dictionary(info, mode):
    install_dir = os.environ['LROSE_INSTALL_DIR']
    core_dir = os.environ['LROSE_CORE_DIR']

    filename = os.path.join(install_dir, 'bin', 'lrose-config.flags')
    f = open(filename, mode)
    for key, value in info.iteritems():
        f.write("#" + key + "\n") 
        for key2, value2 in value.iteritems():
            result = "   " + key + '_' + key2 + '='
            if (len(value2) > 0):
                if (key2.find('lib') > -1):
                    result += formatted(ordered(value2))
                else:
                    result += formatted(value2)
            f.write(result + "\n")
    f.close()

# write to lrose-config.ARCHITECTURE
def write_variables(variable_defs):
    install_dir = os.environ['LROSE_INSTALL_DIR']
    core_dir = os.environ['LROSE_CORE_DIR']

    for key_arch, value_defs in variable_defs.iteritems():
        filename = os.path.join(install_dir, 'bin', 'lrose-config.' + key_arch)
        f = open(filename, 'w')
        f.write("#! /bin/sh\n")
        f.write("prefix=" + install_dir + "\n")

        tmp = os.path.join("includedir=${prefix}", "include")
        f.write(tmp + "\n")

# TODO: need to capture CC and CXX compilers
#       f.write("cc=" + install_dir + "\n")

#       f.write("cflags=" + install_dir + "\n")

#       f.write("libs=" + install_dir + "\n")

        f.write("version=notImplementedYet\n")
        f.write("debug=notImplementedYet\n")
#       f.write("cxx=" + install_dir + "\n")

#       f.write("cxxflags=" + install_dir + "\n")

#       f.write("cxxlibs=" + install_dir + "\n")

#prefix=/h/eol/brenda/lrose
#exec_prefix=${prefix}
        tmp = os.path.join("libdir=${exec_prefix}", "lib")
        f.write(tmp + "\n")


#cc="gcc"
#cflags="-I${includedir} -I/h/eol/brenda/lrose/include"
#libs="-L${libdir} -lnetcdf"

        # print >> sys.stdout, "# architecture-" + key_arch
        for key, value2 in value_defs.iteritems():
            result = key + "="
            if (len(value2) > 0):
                result += formatted(value2)                                     
                # print >> sys.stdout, result
                f.write(result+'\n')
        # print >> sys.stdout, " "
        f.write('\nsource lrose-config.flags')
        f.write('\nsource lrose-config.template')
        f.close()


def write_the_script(variables,idict_libs, idict_apps):
# write output to files that are "included" in the shell script framwork
# write the entire file and keep the template as strings in python file? No.
    write_variables(variable_defs)  # write to separate files based on architecture
    write_dictionary(idict_libs, "w")    # write to one output file
#    write_dictionary(idict_apps, "a")
# TODO: need to put these through ordered; may not need these at all, since we aren't using apps; I can just rename them to libs, cflags, i.e. the common ones
    write_common_flags(idict_libs, idict_apps)

# create a dictionary with keys of target architecture and
#                          values that are dictionaries with keys that are variable names
#                                                            values are lists of strings
def get_variables(variables_to_find, path):
    variable_defs = {}
    filelist = glob.glob(path + '/rap_make.*')
    for file in filelist:
        filename, file_extension = os.path.splitext(file)
        var_defs_for_architecture = {}
        var_defs_for_architecture = getKeyValuesFromFile(file, variables_to_find)
        #for var in variables_to_find:
        #    print >> sys.stdout, "looking for var " + var
        #    valList = getValueListForKey(file, var) # "NETCDF4_LIBS")   
        #    var_defs_for_architecture[var] = valList
        variable_defs[file_extension.replace(".","")] = var_defs_for_architecture

#    for key_arch, value_defs in variable_defs.iteritems():
#        print >> sys.stdout, "architecture-" + key_arch
#        for key, value2 in value_defs.iteritems():
#            result = key + "="
#            if (len(value2) > 0):
#                result += formatted(value2)
#                formatted = '\"' + " ".join(value2) + '\"'
#                print >> sys.stdout, result
#        print >> sys.stdout, " "
    return variable_defs

def detect_variable(astring):
# ^\$[(]   matches $( at the beginning of the string
# (\w+)    captures any following alphanumeric characters and the underscore
    m2 = re.match("^\$[(](\w+)", astring)
    if m2:
        return m2.group(1)
 


def main(path, module_keyword):
    info = {} # info is a dictionary with module names as keys
    loc_includes = set()
    loc_libs = set()
    loc_ldflags = set()
    loc_cflags = set()

    all_includes = set()
    all_libs = set()
    all_ldflags = set()
    all_cflags = set()

    current_module = ""
    variables_to_retrieve = set()
    makefileNames = ["makefile", "Makefile", "_makefile"]
    rootDir = path + '/.'
    for root, dirs, files in os.walk(rootDir):  # , topdown=False):
        # print >> sys.stdout, "root dir: " + root
        # don't need dirs at this time, because we will hit them the
        # next time around as root
	# for dir in dirs:
        #    print >> sys.stdout, "dir: " + dir
        #    dirPath = os.path.join(root, dir)

        # check if makefile exists, extract info if possible                    
                                            
        for name in makefileNames:
            makefileName = os.path.join(root, name)
            if (os.path.isfile(makefileName)):
                # print >>sys.stdout, "Extracting from " + makefileName

                # search for MODULE_NAME key in makefile
                valList = getValueListForKey(makefileName, module_keyword) # "MODULE_NAME")
                if (len(valList) > 0):
                    # print >> sys.stdout, "New Module ", valList[0]
                    # store the current loc sets with the current module
                    if (current_module != ""):
                        info[current_module] = {"includes":loc_includes, "libs":loc_libs, "ldflags":loc_ldflags, "cflags":loc_cflags}
                    current_module = valList[0]
                    loc_includes = set()
                    loc_libs = set()
                    loc_ldflags = set()
                    loc_cflags = set()

#                else:
#                    valList = getValueListForKey(makefileName, "TARGET_FILE")
#                    if (len(valList) > 0):
#                        print >> sys.stdout, "New Target file ", valList[0]
#                    # store the current loc sets with the current module                                  
#                        if (current_module != ""):
#                            info[current_module] = {"includes":loc_includes, "libs":loc_libs, "ldflags":loc_ldflags, "cflags":loc_cflags}
#                        current_module = valList[0]
#                        loc_includes = set()
#                        loc_libs = set()
#                        loc_ldflags = set()
#                        loc_cflags = set()


                valList = getValueListForKey(makefileName, "LOC_LIBS")
                # print >> sys.stdout, "valList " 
                for item in valList:
                    # print >> sys.stdout, item
                    potential_variable = detect_variable(item)
                    if (potential_variable):
                        # stuff it into the variable list to retrieve later
                        variables_to_retrieve.add(potential_variable)
                    loc_libs.add(item)
                    all_libs.add(item)

                valList = getValueListForKey(makefileName, "LOC_INCLUDES")                   
                # print >> sys.stdout, "valList "                         
                for item in valList:
                    # print >> sys.stdout, item   
                    potential_variable = detect_variable(item)
                    if (potential_variable):
                        # stuff it into the variable list to retrieve later 
                        variables_to_retrieve.add(potential_variable)
                    loc_includes.add(item)
                    all_includes.add(item)

                valList = getValueListForKey(makefileName, "LOC_LDFLAGS")                  
                # print >> sys.stdout, "valList "                                                     
                for item in valList:
                    # print >> sys.stdout, item
                    potential_variable = detect_variable(item)
                    if (potential_variable):
                        # stuff it into the variable list to retrieve later 
                        variables_to_retrieve.add(potential_variable)          
                    loc_ldflags.add(item)
                    all_ldflags.add(item)

                valList = getValueListForKey(makefileName, "LOC_CFLAGS")                
                # print >> sys.stdout, "valList "                                                       
                for item in valList:
                    # print >> sys.stdout, item                      
                    potential_variable = detect_variable(item)
                    if (potential_variable):
                        # stuff it into the variable list to retrieve later            
                        variables_to_retrieve.add(potential_variable)                                 
                    loc_cflags.add(item)
                    all_cflags.add(item)


    # store the current loc sets with the current module                                  
    if (current_module != ""):
        info[current_module] = {"includes":loc_includes, "libs":loc_libs, "ldflags":loc_ldflags,"cflags":loc_cflags}

    # store the sets for all apps or all libs as 'apps' or 'libs' key; extract from the path
    libapp_key = os.path.basename(path)
    info[libapp_key] =  {"includes":all_includes, "libs":all_libs, "ldflags":all_ldflags, "cflags":all_cflags}


#    print >> sys.stdout, "at the end: "
#    print >> sys.stdout, "variables found: ", variables_to_retrieve
#    print >> sys.stdout, "loc_libs = "
#    print >> sys.stdout, loc_libs

#    print >> sys.stdout, "loc_includes = "
#    print >> sys.stdout, loc_includes

#    print >> sys.stdout, "loc_ldflags = "
#    print >> sys.stdout, loc_ldflags

#    print >> sys.stdout, "loc_cflags = "
#    print >> sys.stdout, loc_cflags

#    write_dictionary(info)

#    for key, value in info.iteritems():
#        print >> sys.stdout, key 
#        for key2, value2 in value.iteritems():
#            result = "   " + key + '_' + key2 + '='
#            if (len(value2) > 0):
#                result += formatted(value2)
#            print >> sys.stdout, result

    return variables_to_retrieve, info


def formatted(set_or_list):
    if (len(set_or_list) >= 0):
        result =  '\"' + " ".join(set_or_list) + '\"'
        result = result.replace("(", "{")
        result = result.replace(")", "}")
        return result
    else:
        return

#
# order the libraries based on the file defining the order
# ..... lrose-core/docs/lrose_libs_order.txt
# return the list of libraries in the correct order
def ordered(set_or_list):
    orderedList = []

    # read the prescribed order for the libraries
    # and create a dictionary of them, with the number of votes as their value
    order = []
    try:
        fp = open('../docs/lrose_libs_order.txt', 'r')
    except IOError as e:
        print >>sys.stdout, "ERROR - ", thisScriptName
        print >>sys.stdout, "  Cannot open file:", path
        print >>sys.stdout, "  dir: ", options.dir
        return orderedList

    lines = fp.readlines()
    fp.close()

    for line in lines:
        tokens = line.split()
        if (len(tokens) > 0):
            order.append(tokens[0])

    votes = [0] * len(order)

    for lib in set_or_list:
        try:
            idx = order.index(lib)
            votes[idx] = 1
        except ValueError as e:        
            print >> sys.stderr, "WARNING - ", thisScriptName
            print >> sys.stderr, "  Library not found in expected list:" , lib

    for idx, val in enumerate(votes):
#    print(idx, val)
        if (val > 0):
            orderedList.append(order[idx])

#    print >> sys.stderr, "original: ", set_or_list
#    print >> sys.stderr, " ordered: ", orderedList
    
    return orderedList

# get string value based on search key
# the string may span multiple lines
#
# Example of keys: SRCS, SUB_DIRS, MODULE_NAME, TARGET_FILE
#
# value is returned

def getValueListForKey(path, key):

    valueList = []

    try:
        fp = open(path, 'r')
    except IOError as e:
        print >>sys.stdout, "ERROR - ", thisScriptName
        print >>sys.stdout, "  Cannot open file:", path
        print >>sys.stdout, "  dir: ", options.dir
        return valueList

    lines = fp.readlines()
    fp.close()

    foundKey = False
    multiLine = ""
    for line in lines:
        if (foundKey == False):
            if (line[0] == '#'):
                continue
        if (line.find(key) >= 0):
            foundKey = True
            multiLine = multiLine + line
            if (line.find("\\") < 0):
                break;
        elif (foundKey == True):
            if (line[0] == '#'):
                break
            if (len(line) < 2):
                break
            multiLine = multiLine + line;
            if (line.find("\\") < 0):
                break;

    if (foundKey == False):
        return valueList

    multiLine = multiLine.replace(key, " ",1)
    multiLine = multiLine.replace("=", " ")
    multiLine = multiLine.replace("\t", " ")
    multiLine = multiLine.replace("\\", " ")
    multiLine = multiLine.replace("\r", " ")
    multiLine = multiLine.replace("\n", " ")

    toks = multiLine.split(' ')
    for tok in toks:
        if (len(tok) > 0):
            valueList.append(tok)

    return valueList



########################################################################

# parse each line of the file, if the variable is a key in the list
# stuff the key, value pair into a dictionary
# return the dictionary

def parseLine(line, keys):
    found = False
    tokens = line.split()
    ikey = ''
    value = []
    if (len(tokens) >= 2) and (tokens[0] in keys) and (tokens[1] == '='): 
        # then we have at least '<var> =' and it's a variable we want
        ikey = tokens[0]
        # the remainder of the list is the value of the variable 
        value = tokens[2:]
        # valueList[ikey] = value
        found = True
    return found, ikey, value


def getKeyValuesFromFile(path, keys):

    valueList = {} # []

    try:
        fp = open(path, 'r')
    except IOError as e:
        print >>sys.stdout, "ERROR - ", thisScriptName
        print >>sys.stdout, "  Cannot open file:", path
        print >>sys.stdout, "  dir: ", options.dir
        return valueList

    lines = fp.readlines()
    fp.close()

    foundKey = False
    multiLine = ""
    linesForReal = []

    for line in lines:
        # first of all, accumulate multiple lines, then parse
        if (line[0] != '#'):  # only consider lines that aren't comments
        
            multiLine = multiLine + line

            if (line.find("\\") < 0):  # not a continuation
                linesForReal.append(multiLine)
                multiLine = ""

#  -----------
#        if (foundKey == False):
#            if (line[0] == '#'):
#                continue
#        if (line.find(key) >= 0):
#            foundKey = True
#            multiLine = multiLine + line
#            if (line.find("\\") < 0):
#                break;
#        elif (foundKey == True):
#            if (line[0] == '#'):
#                break
#            if (len(line) < 2):
#                break
#            multiLine = multiLine + line;
#            if (line.find("\\") < 0):
#                break;

    for line in linesForReal:
        found, key, value = parseLine(line, keys)
        if (found):
            valueList[key] = value

    return valueList

    # all the white space characters should have been removed by
    # the tokenizer, split
    #multiLine = multiLine.replace(key, " ",1)
    #multiLine = multiLine.replace("=", " ")
    #multiLine = multiLine.replace("\t", " ")
    #multiLine = multiLine.replace("\\", " ")
    #multiLine = multiLine.replace("\r", " ")
    #multiLine = multiLine.replace("\n", " ")

    #toks = multiLine.split(' ')
    #for tok in toks:
    #    if (len(tok) > 0):
    #        valueList.append(tok)

    #return valueList



########################################################################


                                                                          
# Run - entry point

if __name__ == "__main__":
    # path = './make_include'
    # get_variables(["NETCDF4_LIBS", "NETCDF4_LDFLAGS", "NETCDF4_INC"], path)
    # apps_path = './apps'
    # module_keyword = 'TARGET_FILE'
    # idict = main(apps_path, module_keyword)

    thisScriptName = os.path.basename(__file__)


    libs_path = './libs'
    module_keyword = 'MODULE_NAME'
    variables_libs, idict_libs = main(libs_path, module_keyword)
    # path = './make_include'
    # get_variables(variables, path)

    apps_path = './apps'                                                                               
    module_keyword = 'TARGET_FILE'                                                                     
    variables_apps, idict_apps = main(apps_path, module_keyword)  

    path = './make_include'
    variables = variables_libs.union(variables_apps)
    # 'variables' contains a list of variables ($(var)) that we need expansion
    # their values should be in the make_include/rap_make.$(ARCH) files
    #  but, let's add our own set of variables to expand ...
    addtnl_vars = set(['CC', 'CPPC', 'FC', 'F90C'])
    variables = variables.union(addtnl_vars)
    variable_defs = get_variables(variables, path)

    write_the_script(variable_defs, idict_libs, idict_apps)
