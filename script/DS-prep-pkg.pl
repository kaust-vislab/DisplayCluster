################################################################################
# This script prepares the DesktopStreamer.pkg file given the DesktopStreamer.app 
# directory. The package should contain all the library dependencies required 
# for running the application.
# 
# This script performs the following functions:
# 1. Create subdirectories for carrying the library framework files
# 2. Copy the required framework and library files to the created directories
# 3. Changes the path in all binary files to be relative to the executable
# 4. Create a .pkg file for distribution
#
# Problems Faced and Decisions Taken (aka Why I Did What I Did):
# 
# Definitions and Terms:
#   exec:
#   bin:
#   path:
#   list:
#   lib:
#   Framework:
#   dst:
#   app:
#   dep:
#
################################################################################


# Assuming that the path to DesktopStreamer.app is given as command line arg
my $app_path = shift(@ARGV);

###########
# Create the subdirectories
##########
my $dst_framework_path = $app_path . "/Contents/Frameworks";
mkdir $dst_framework_path,0777;

my $dst_lib_path = $app_path . "/Contents/lib";
mkdir($dst_lib_path,0777);

my $res_path = $app_path . "/Contents/Resources";

##########
# Get all the relevant framework and library files
##########
# Start by getting the dependency list from the executable
my $exec_path = $app_path . "/Contents/MacOS";
my $exec = $exec_path . "/DesktopStreamer";

my $bin;
my @bin_list;
my @bin_dep_list;
my $bin_dep_list_count;
my @bin_dep_path_list;
my @bin_dep_name_list;
my @original_path_list;
my $original_path;
my @bin_dir_list;
my $bin_dir;
#my @copy_framework_list;
#my @copy_lib_list;
my @copy_list;
my $cmd;
my $result = 0;

my $count = 0;
my $debug = 1;

# Start by adding the executable to the list. We will process its dependencies first
#   as they will be added to the list and similary processed subsequently
push(@bin_list, $exec);
push(@original_path_list, $exec);
push(@bin_dir_list, "exec");
while(scalar(@bin_list) > 0)
{
    if($debug == 1)
    {
        print("bin_list:\n");
        for(my $a = 0; $a < scalar(@bin_list); $a++)
        {
            print("    $bin_list[$a]\n");
        }
    }

    $bin = shift(@bin_list);
    $original_path = shift(@original_path_list);
    $bin_dir = shift(@bin_dir_list);

    if ($debug == 1)
    {
        print("bin:$bin\n");
        print("original_path:$original_path\n");
        print("bin_dir:$bin_dir\n");
    }

    # Use the Mac OS X tool to fetch the dependency list
    @bin_dep_list = `otool -L $bin`;

    if ($debug == 1)
    {
        for ($a = 0; $a < scalar(@bin_dep_list); $a++) {
            print("$bin_dep_list[$a]");
        }
    }

    # Delete the first element as that is not needed
    shift(@bin_dep_list);
    # Determine the total number of dependencies
    $bin_dep_list_count = scalar(@bin_dep_list);
    # Process each entry in the dependency list into a path and name of binary
    #   at the same time removing leading spaces and trailing junk
    my $i = 0;
    my $line;
    my @line_parts;
    my $dep;
    my @dep_path;
    my $dep_name_index = 0;
    my $copy_dep;
    for ($i = 0; $i < $bin_dep_list_count; $i++)
    {
        # Get dependency from list
        $line = $bin_dep_list[$i];
        # Remove leading whitespaces
        $line =~ s/^\s+//;
        # Separate the line into parts delimited by ' '
        @line_parts = split(' ', $line);
        # The first part is path to a file and rest is not needed for our purposes
        $dep = $line_parts[0];
        # Break up the path of dependency into components
        @dep_path = split('/', $dep);
        # Since all paths starts with '/' element $dep_path[0] is null, so get rid of it
        shift(@dep_path);
        # The actual filename is the last element
        $dep_name_index = scalar(@dep_path)-1;
        # Is this self-referential dependency?
        if ($dep eq $original_path)
        {
            $cmd = "install_name_tool -id \@executable_path/../$bin_dir/$dep_path[$dep_name_index] $bin";
            print "$cmd\n";
            $result = `$cmd`;
        }
        else  # Nope this is some other dependency
        {
            my $is_framework = 0;
            # Separate out external dependencies from system dependencies and copy them to 
            #   appropriate directory
            if (!(($dep_path[0] eq "System") ||
                  ($dep_path[0] eq "Library") ||
                  (($dep_path[0] eq "usr") && ($dep_path[1] eq "lib"))))
            {
                # Has the dependency already been copied? 
                my $j = 0;
                my $copied = 0;
                my $dst_path;
                my $dst_dir;
                for ($j = 0; $j < scalar(@copy_list); $j++)
                {
                    if ($dep eq $copy_list[$j])
                    {
                        $copied = 1;
                        break;
                    }
                }

                # Copy framework or library directory
                $is_framework = 0;
                my $k = 0;
                for ($k = 0; $k < scalar(@dep_path); $k++)
                {
                    if (($dep_path[$k] eq "Frameworks") ||
                        ($dep_path[$k] eq "Framework"))
                    {
                        $is_framework = 1;
                    }
                }
                if ($is_framework == 1)
                {
                    $dst_dir = "Frameworks";
                }
                else
                {
                    $dst_dir = "lib";
                }

                if ($copied == 0)
                {
                    if ($is_framework == 1)
                    {
                        $dst_path = $dst_framework_path;
                    }
                    else
                    {
                        $dst_path = $dst_lib_path;
                    }
                    $cmd = "cp $dep $dst_path";
                    print "$cmd\n";
                    $result = `$cmd`;

                    $cmd = "chmod +w $dst_path/$dep_path[$dep_name_index]";
                    print "$cmd\n";
                    $result = `$cmd`;

                    $copy_dep = "$dst_path/$dep_path[$dep_name_index]";
                    print "copy_dep:$copy_dep\n";

                    push(@bin_list, $copy_dep);
                    push(@copy_list, $dep);
                    push(@original_path_list, $dep);
                    push(@bin_dir_list, $dst_dir);
                }

                #for (my $l = 0; $l < scalar(@dep_path); $l++)
                #{
                #    print "dep_path[$l]:$dep_path[$l]\n";
                #}
                #print "$dep_name_index\n";
                #print "$dep_path[$dep_name_index]\n";

                $cmd = "install_name_tool -change $dep \@executable_path/../$dst_dir/$dep_path[$dep_name_index] $bin";
                print "$cmd\n";
                $result = `$cmd`;
            }
        }

        # TODO: fix the path in the current binary
        

        ##########
        # TODO: Determine if the following section is needed
        #$last_slash_index = rindex($dep, '/');
        #$bin_dep_path_list[$i] = substr($dep[0], 0, $last_slash_index+1);
        #$bin_dep_name_list[$i] = substr($dep[0], ($last_slash_index-length($dep[0])+1));

        # Look for copying needed from frameworks
        #if (($bin_dep_name_list[$i] eq "QtCore") || 
        #    ($bin_dep_name_list[$i] eq "QtGui"))
        #{
        #    push(@copy_list, $dep[0]);
        #    push(@copy_framework_list, $dep[0]);
        #}
        #elsif (($bin_dep_name_list[$i] eq "libdcstream.0.dylib") ||
        #    ($bin_dep_name_list[$i] eq "libboost_system-mt.dylib"))
        #{
        #    push(@copy_list, $dep[0]);
        #    push(@copy_lib_list, $dep[0]);
        #}
        ##########
    }
    $count++;
    print "\n\n";
}

# To prevent Qt from loading pre-installed libraries and instead use the binaries provided 
#   with the package, we need to create a dummy qt.conf file under Resources
$cmd = "touch $res_path/qt.conf";
print "$cmd\n";
$result = `$cmd`;

