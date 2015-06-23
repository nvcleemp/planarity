# Linux #

  1. Team Update
  1. Build release
  1. Copy planarity.exe and samples directory to ~user/bin
  1. dos2unix samples directory
  1. zip -r planarity.vNNN.linuxIntel.zip planarity.exe samples
  1. Open zip file and remove any svn files
  1. Unit test with "./planarity.exe -test"
  1. Deprecate old Linux build
  1. Upload new version
  1. Add labels Type-Archive and OpSys-Linux