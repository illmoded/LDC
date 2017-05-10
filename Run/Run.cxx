void Run(const std::string &submitDir)
{
  //===========================================
  // FOR ROOT6 WE DO NOT PUT THIS LINE
  // (ROOT6 uses Cling instead of CINT)
  // Load the libraries for all packages
  // gROOT->Macro("$ROOTCOREDIR/scripts/load_packages.C");
  // Instead on command line do:
  // > root -l '$ROOTCOREDIR/scripts/load_packages.C' 'ATestRun.cxx ("submitDir")'
  // The above works for ROOT6 and ROOT5
  //==========================================

  // Set up the job for xAOD access:
  xAOD::Init().ignore();

  // create a new sample handler to describe the data files we use
  SH::SampleHandler sh;

  // scan for datasets in the given directory
  // this works if you are on lxplus, otherwise you'd want to copy over files
  // to your local machine and use a local path.  if you do so, make sure
  // that you copy all subdirectories and point this to the directory
  // containing all the files, not the subdirectories.

  // use SampleHandler to scan all of the subdirectories of a directory for particular MC single file:

  // // *******************************
  //   //
  //   //const char* inputFilePath = gSystem->ExpandPathName ("/afs/cern.ch/user/p/pbuglewi/public/AFP/LDCorr/data");
  //  const char* inputFilePath = gSystem->ExpandPathName ("/afs/cern.ch/work/k/kciesla/public/run310216_skimmed_mbts");
  //    // const char* inputFilePath = gSystem->ExpandPathName ("/afs/cern.ch/work/p/pbuglewi/public/data1/data16_13TeV");

  // const char* inputFilePath = gSystem->ExpandPathName("/eos/user/r/rafal/AFP/user.rafal.00310216.r8600.TrigSkim.HLT_mb_sp900_trk50_hmt_L1TE5.v01_skimmed.root");
  const char *inputFilePath = gSystem->ExpandPathName("/eos/user/r/rafal/AFP/user.rafal.00310216.r9017.TrigSkim.HLT_mb_sptrk_trk80_L1AFP_C_AND.v01_skimmed.root");

  SH::ScanDir()
      // // //    // //.filePattern("user.sczekier.10576193._00000*")
      .filePattern("*root*")
      //    // .filePattern("*0001*.skimmed.root")
      //
      .scan(sh, inputFilePath);
  // .scanEOS(sh,inputFilePath);

  // // *******************************

  // *******************************
  // FAX test -_-
  // SH::scanDQ2 (sh, "data16_13TeV:data16_13TeV.00310216.physics_MinBias.recon.AOD.r8600_tid09604852_00");
  // nie dziala

  // SH::scanDQ2 (sh,"data16_13TeV:data16_13TeV.00310216.physics_MinBias.recon.AOD.r9017_tid10468087_00");

  // *******************************

  // set the name of the tree in our files
  // in the xAOD the TTree containing the EDM containers is "CollectionTree"
  sh.setMetaString("nc_tree", "CollectionTree");

  // further sample handler configuration may go here

  // print out the samples we found
  sh.print();

  // this is the basic description of our job
  EL::Job job;
  job.sampleHandler(sh); // use SampleHandler in this job
  //  job.options()->setDouble (EL::Job::optMaxEvents, 500); // for testing purposes, limit to run over the first 500 events only!

  // add our algorithm to the job
  LDCAna *alg = new LDCAna;

  // later on we'll add some configuration options for our algorithm that go here

  job.algsAdd(alg);
  job.options()->setDouble(EL::Job::optRetries, 3);
  // job.options()->setDouble (EL::Job::optFilesPerWorker, 5);

  // job.options()->setDouble (EL::Job::optSkipEvents, 3000000); //skip to
  // job.options()->setDouble (EL::Job::optMaxEvents, 2000000); //max

  // make the driver we want to use:
  // this one works by running the algorithm directly:
  EL::DirectDriver driver;
  // we can use other drivers to run things on the Grid, with PROOF, etc.

  // process the job using the driver
  driver.submit(job, submitDir);
}
