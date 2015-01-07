################################################################################
# This script prepares the DesktopStreamer.pmdoc file given the 
# DesktopStreamer.app directory. The package should contain all the library 
# dependencies required for running the application.
# 
# This script performs the following functions:
# 1. Create subdirectories for carrying the framework and library files
# 2. Copy the required framework and library files to the created directories
# 3. Changes the path in all binary files to be relative to the executable
# 4. Create a .pmdoc directory to be used to build .pkg using PackageMaker
#
# Commandline:
# $ perl DS-perp-pkg.pl <path>/DesktopStreamer.app
################################################################################

use strict;
use warnings;
use 5.010;

# Special variables
# $debug = 1 will output a lot of text to stdout
my $debug = 0;
# $readable_pmdoc will generate XML files which are nicely formatted for humans
my $readable_pmdoc = 0;

# Assuming that the path to DesktopStreamer.app is given as command line arg
my $app_path = shift(@ARGV);

########################################
# Create the subdirectories
########################################
# Create Frameworks subdirectory
my $dst_framework_path = $app_path . "/Contents/Frameworks";
mkdir $dst_framework_path,0777;
# Create lib subdirectory
my $dst_lib_path = $app_path . "/Contents/lib";
mkdir($dst_lib_path,0777);
# Path to Resources subdirectory
my $res_path = $app_path . "/Contents/Resources";

########################################
# Get all the relevant framework and library files
########################################
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
my @copy_list;
my $cmd;
my $result = 0;
my $count = 0;

# Start by adding the executable to the list. We will process its dependencies 
#   first as they will be added to the list and similary processed subsequently
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
            if ($debug == 1)
            {
                print "$cmd\n";
            }
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
                        #break;
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
                    if ($debug == 1)
                    {
                        print "$cmd\n";
                    }
                    $result = `$cmd`;

                    $cmd = "chmod +w $dst_path/$dep_path[$dep_name_index]";
                    if ($debug == 1)
                    {
                        print "$cmd\n";
                    }
                    $result = `$cmd`;

                    $copy_dep = "$dst_path/$dep_path[$dep_name_index]";
                    if ($debug == 1)
                    {
                        print "copy_dep:$copy_dep\n";
                    }

                    push(@bin_list, $copy_dep);
                    push(@copy_list, $dep);
                    push(@original_path_list, $dep);
                    push(@bin_dir_list, $dst_dir);
                }

                $cmd = "install_name_tool -change $dep \@executable_path/../$dst_dir/$dep_path[$dep_name_index] $bin";
                if ($debug == 1)
                {
                    print "$cmd\n";
                }
                $result = `$cmd`;
            }
        }
    }
    $count++;

    if ($debug == 1)
    {
        print "\n\n";
    }
}

# To prevent Qt from loading pre-installed libraries and instead use the 
#   binaries provided with the package, we need to create an empty  qt.conf 
#   file under Resources
$cmd = "touch $res_path/qt.conf";
if ($debug == 1)
{
    print "$cmd\n";
}
$result = `$cmd`;


########################################
# Generate pmdoc files to run PackageMaker on
########################################
# Get parent directory of the app directory
my $app_path_last_sep_index = rindex($app_path, "/");
my $app_parent_path = substr($app_path, 0, $app_path_last_sep_index);
if ($debug == 1)
{
    print "app_parent_path:$app_parent_path\n";
}
# Get the name of the app
my $app_name = substr($app_path, $app_path_last_sep_index+1, length($app_path)-$app_path_last_sep_index-1);
if ($debug == 1)
{
    print "app_name:$app_name\n";
}
# Get name without the .app suffix
my $app_name_no_suffix;
if (substr($app_name, -4) eq ".app")
{
    $app_name_no_suffix = substr($app_name, 0, length($app_name)-length(".app"));
}
# Use the path and name without suffix to generate the path for .pmdoc
my $pmdoc_path = $app_parent_path . "/$app_name_no_suffix" . ".pmdoc";
if ($debug == 1)
{
    print "pmdoc_path:$pmdoc_path\n";
}
# Create pmdoc directory
$cmd = "mkdir $pmdoc_path";
$result = `$cmd`;
# Generate .pkg path
my $pkg_file = $app_parent_path . "/$app_name_no_suffix.pkg";
# Generate pmdoc index xml file path
my $pmdoc_index_file = $pmdoc_path . "/index.xml";
# Generate pmdoc config xml file path
my $pmdoc_config_file_name_no_suffix = "01" . lc($app_name_no_suffix);
my $pmdoc_config_file_name = $pmdoc_config_file_name_no_suffix . ".xml";
my $pmdoc_config_file = $pmdoc_path . "/$pmdoc_config_file_name";
if ($debug == 1)
{
    print "pmdoc_config_file:$pmdoc_config_file\n";
}
# Generate pmdoc contents xml file path
my $pmdoc_contents_file_name = $pmdoc_config_file_name_no_suffix . "-contents.xml";
my $pmdoc_contents_file = $pmdoc_path . "/$pmdoc_contents_file_name";
# Generate index xml file
open(INDEX_OUT, ">", $pmdoc_index_file) or die "Couldn't open index.xml file for pmdoc generation";
print INDEX_OUT "<pkmkdoc spec=\"1.12\">";
if ($readable_pmdoc == 1) { print INDEX_OUT "\n"; }
if ($readable_pmdoc == 1) { print INDEX_OUT "    "; }
print INDEX_OUT "<properties>";
if ($readable_pmdoc == 1) { print INDEX_OUT "\n"; }
if ($readable_pmdoc == 1) { print INDEX_OUT "        "; }
print INDEX_OUT "<title>$app_name_no_suffix</title>";
if ($readable_pmdoc == 1) { print INDEX_OUT "\n"; }
if ($readable_pmdoc == 1) { print INDEX_OUT "        "; }
print INDEX_OUT "<build>$pkg_file</build>";
if ($readable_pmdoc == 1) { print INDEX_OUT "\n"; }
if ($readable_pmdoc == 1) { print INDEX_OUT "        "; }
print INDEX_OUT "<organization>edu.kaust.kvl</organization>";
if ($readable_pmdoc == 1) { print INDEX_OUT "\n"; }
if ($readable_pmdoc == 1) { print INDEX_OUT "        "; }
print INDEX_OUT "<userSees ui=\"easy\"/>";
if ($readable_pmdoc == 1) { print INDEX_OUT "\n"; }
if ($readable_pmdoc == 1) { print INDEX_OUT "        "; }
print INDEX_OUT "<min-target os=\"3\"/>";
if ($readable_pmdoc == 1) { print INDEX_OUT "\n"; }
if ($readable_pmdoc == 1) { print INDEX_OUT "        "; }
print INDEX_OUT "<domain anywhere=\"true\"/>";
if ($readable_pmdoc == 1) { print INDEX_OUT "\n"; }
if ($readable_pmdoc == 1) { print INDEX_OUT "    "; }
print INDEX_OUT "</properties>";
if ($readable_pmdoc == 1) { print INDEX_OUT "\n"; }
if ($readable_pmdoc == 1) { print INDEX_OUT "    "; }
print INDEX_OUT "<distribution>";
if ($readable_pmdoc == 1) { print INDEX_OUT "\n"; }
if ($readable_pmdoc == 1) { print INDEX_OUT "        "; }
print INDEX_OUT "<versions min-spec=\"1.000000\"/>";
if ($readable_pmdoc == 1) { print INDEX_OUT "\n"; }
if ($readable_pmdoc == 1) { print INDEX_OUT "        "; }
print INDEX_OUT "<scripts></scripts>";
if ($readable_pmdoc == 1) { print INDEX_OUT "\n"; }
if ($readable_pmdoc == 1) { print INDEX_OUT "    "; }
print INDEX_OUT "</distribution>";
if ($readable_pmdoc == 1) { print INDEX_OUT "\n"; }
if ($readable_pmdoc == 1) { print INDEX_OUT "    "; }
print INDEX_OUT "<contents>";
if ($readable_pmdoc == 1) { print INDEX_OUT "\n"; }
if ($readable_pmdoc == 1) { print INDEX_OUT "        "; }
print INDEX_OUT "<choice title=\"$app_name_no_suffix\" id=\"choice0\" starts_selected=\"true\" starts_enabled=\"true\" starts_hidden=\"false\">";
if ($readable_pmdoc == 1) { print INDEX_OUT "\n"; }
if ($readable_pmdoc == 1) { print INDEX_OUT "            "; }
print INDEX_OUT "<pkgref id=\"edu.kaust.kvl.$app_name_no_suffix.pkg\"/>";
if ($readable_pmdoc == 1) { print INDEX_OUT "\n"; }
if ($readable_pmdoc == 1) { print INDEX_OUT "        "; }
print INDEX_OUT "</choice>";
if ($readable_pmdoc == 1) { print INDEX_OUT "\n"; }
if ($readable_pmdoc == 1) { print INDEX_OUT "    "; }
print INDEX_OUT "</contents>";
if ($readable_pmdoc == 1) { print INDEX_OUT "\n"; }
if ($readable_pmdoc == 1) { print INDEX_OUT "    "; }
print INDEX_OUT "<resources bg-scale=\"none\" bg-align=\"topleft\">";
if ($readable_pmdoc == 1) { print INDEX_OUT "\n"; }
if ($readable_pmdoc == 1) { print INDEX_OUT "        "; }
print INDEX_OUT "<locale lang=\"en\"/>";
if ($readable_pmdoc == 1) { print INDEX_OUT "\n"; }
if ($readable_pmdoc == 1) { print INDEX_OUT "    "; }
print INDEX_OUT "</resources>";
if ($readable_pmdoc == 1) { print INDEX_OUT "\n"; }
if ($readable_pmdoc == 1) { print INDEX_OUT "    "; }
print INDEX_OUT "<flags/>";
if ($readable_pmdoc == 1) { print INDEX_OUT "\n"; }
if ($readable_pmdoc == 1) { print INDEX_OUT "    "; }
print INDEX_OUT "<item type=\"file\">$pmdoc_config_file_name</item>";
if ($readable_pmdoc == 1) { print INDEX_OUT "\n"; }
#if ($readable_pmdoc == 1) { print INDEX_OUT "    "; }
#print INDEX_OUT "<mod>properties.title</mod>";
#if ($readable_pmdoc == 1) { print INDEX_OUT "\n"; }
#if ($readable_pmdoc == 1) { print INDEX_OUT "    "; }
#print INDEX_OUT "<mod>properties.userDomain</mod>";
#if ($readable_pmdoc == 1) { print INDEX_OUT "\n"; }
#if ($readable_pmdoc == 1) { print INDEX_OUT "    "; }
#print INDEX_OUT "<mod>properties.systemDomain</mod>";
#if ($readable_pmdoc == 1) { print INDEX_OUT "\n"; }
print INDEX_OUT "</pkmkdoc>";
close(INDEX_OUT);
# Generate config xml file
open(CONFIG_OUT, ">", $pmdoc_config_file) or die "Couldn't open $pmdoc_config_file_name for pmdoc generation";
print CONFIG_OUT "<pkgref spec=\"1.12\" uuid=\"285B768F-947A-40AA-BBDE-35122F9C0F5D\">";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "    "; }
print CONFIG_OUT "<config>";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "        "; }
print CONFIG_OUT "<identifier>edu.kaust.kvl.$app_name_no_suffix.pkg</identifier>";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "        "; }
print CONFIG_OUT "<version>1.0</version>";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "        "; }
print CONFIG_OUT "<description/>";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "        "; }
print CONFIG_OUT "<requireAuthorization/>";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "        "; }
print CONFIG_OUT "<post-install type=\"none\"/>";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "        "; }
print CONFIG_OUT "<installFrom includeRoot=\"true\">$app_path</installFrom>";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "        "; }
print CONFIG_OUT "<installTo relocatable=\"true\">/Applications</installTo>";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "        "; }
print CONFIG_OUT "<flags>";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "            "; }
print CONFIG_OUT "<followSymbolicLinks/>";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "        "; }
print CONFIG_OUT "</flags>";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "        "; }
print CONFIG_OUT "<packageStore type=\"internal\"/>";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
#if ($readable_pmdoc == 1) { print CONFIG_OUT "        "; }
#print CONFIG_OUT "</packageStore>";
#if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "        "; }
print CONFIG_OUT "<mod>requireAuthorization</mod>";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "        "; }
print CONFIG_OUT "<mod>identifier</mod>";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "        "; }
print CONFIG_OUT "<mod>parent</mod>";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "    "; }
print CONFIG_OUT "</config>";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "    "; }
print CONFIG_OUT "<contents>";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "        "; }
print CONFIG_OUT "<file-list>$pmdoc_contents_file_name</file-list>";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "        "; }
print CONFIG_OUT "<component id=\"\" path=\"$app_path\" version=\"\" isRelocatable=\"true\">";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "            "; }
print CONFIG_OUT "<locator-info>";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "                "; }
print CONFIG_OUT "<token title=\"pkmk-token-2\">";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "                    "; }
print CONFIG_OUT "<search-rule>";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "                        "; }
print CONFIG_OUT "<combo identifier=\"\" default-path=\"/Applications/$app_name\"/>";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "                    "; }
print CONFIG_OUT "</search-rule>";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "                "; }
print CONFIG_OUT "</token>";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "            "; }
print CONFIG_OUT "</locator-info>";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "        "; }
print CONFIG_OUT "</component>";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "        "; }
print CONFIG_OUT "<filter>/CVS\$</filter>";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "        "; }
print CONFIG_OUT "<filter>/\\.svn\$</filter>";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "        "; }
print CONFIG_OUT "<filter>/\\.cvsignore\$</filter>";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "        "; }
print CONFIG_OUT "<filter>/\\.cvspass\$</filter>";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "        "; }
print CONFIG_OUT "<filter>/\\.DS_Store\$</filter>";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
if ($readable_pmdoc == 1) { print CONFIG_OUT "    "; }
print CONFIG_OUT "</contents>";
if ($readable_pmdoc == 1) { print CONFIG_OUT "\n"; }
print CONFIG_OUT "</pkgref>";
close(CONFIG_OUT);
# Generate contents xml file
my $dev;
my $ino;
my $mode;
my $nlink;
my $uid;
my $gid;
my $rdev;
my $size;
my $atime;
my $mtime;
my $ctime;
my $blksize;
my $blocks;
my $uname;
my @st;
my $grname;
my $grpasswd;
my $grgid;
my $grmembers;
open(CONTENTS_OUT, ">", $pmdoc_contents_file) or die "Couldn't open $pmdoc_contents_file_name for pmdoc generation";
print CONTENTS_OUT "<pkg-contents spec=\"1.12\">";
if ($readable_pmdoc == 1) { print CONTENTS_OUT "\n"; }
# Get directory information of the root including username, group name and permissions
($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks) = stat($app_path);
$uname = getpwuid($uid);
($grname,$grpasswd,$grgid,$grmembers) = getgrgid($gid);
if ($readable_pmdoc == 1) { print CONTENTS_OUT "    "; }
print CONTENTS_OUT "<f n=\"$app_name\" o=\"$uname\" g=\"$grname\" p=\"$mode\" pt=\"$app_path\" m=\"false\" t=\"file\">";
if ($readable_pmdoc == 1) { print CONTENTS_OUT "\n"; }
# Start traversing the root
opendir my $dh, $app_path or die;
while (my $sub = readdir $dh)
{
    next if $sub eq '.' or $sub eq '..';
    traverse("$app_path/$sub", 1);
}
# Subroutine for recursive traversal
sub traverse
{
    my $dev;
    my $ino;
    my $smode;
    my $nlink;
    my $uid;
    my $gid;
    my $rdev;
    my $size;
    my $atime;
    my $mtime;
    my $ctime;
    my $blksize;
    my $blocks;
    my $uname;
    my $grname;
    my $grpasswd;
    my $grgid;
    my $grmembers;
    my $path = shift(@_);
    my $depth = shift(@_);

    my $last_sep_index = rindex($path, '/');
    my $name = substr($path, $last_sep_index+1, length($path)-$last_sep_index-1);
    ($dev,$ino,$smode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks) = stat($path);
    $uname = getpwuid($uid);
    ($grname,$grpasswd,$grgid,$grmembers) = getgrgid($gid);

    if ($readable_pmdoc == 1)
    { 
        print CONTENTS_OUT "    "; 
        for (my $d = 0; $d < $depth; $d++)
        {
            print CONTENTS_OUT "    ";
        }
    }
    print CONTENTS_OUT "<f n=\"$name\" o=\"$uname\" g=\"$grname\" p=\"$smode\"";
    
    if (-d $path)
    {
        # process dir
        print CONTENTS_OUT ">";
        if ($readable_pmdoc == 1) { print CONTENTS_OUT "\n"; }
    }
    else
    {
        # process file
        print CONTENTS_OUT "/>";
        if ($readable_pmdoc == 1) { print CONTENTS_OUT "\n"; }
        return;
    }

    opendir my $dh, $path or die;
    while (my $sub = readdir $dh)
    {
        next if $sub eq '.' or $sub eq '..';
        #say "$path/$sub";
        traverse("$path/$sub", $depth+1);
    }
    close $dh;
    if ($readable_pmdoc == 1)
    {
        print CONTENTS_OUT "    ";
        for (my $d = 0; $d < $depth; $d++)
        {
            print CONTENTS_OUT "    ";
        }
    }
    print CONTENTS_OUT "</f>";
    if ($readable_pmdoc == 1) { print CONTENTS_OUT "\n"; }
    return;
}
if ($readable_pmdoc == 1) { print CONTENTS_OUT "    "; }
print CONTENTS_OUT "</f>";
if ($readable_pmdoc == 1) { print CONTENTS_OUT "\n"; }
print CONTENTS_OUT "</pkg-contents>";
close(CONTENTS_OUT);

