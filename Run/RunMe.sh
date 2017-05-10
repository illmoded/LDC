rm -rf Result

root -l -q '$ROOTCOREDIR/scripts/load_packages.C' 'Run.cxx ("Result")'
