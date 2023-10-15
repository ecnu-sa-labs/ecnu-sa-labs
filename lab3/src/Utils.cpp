#include <Utils.h>

int successCount = 0;
int failureCount = 0;

void initialize(std::string &OutDir) {
  int Status;
  std::string SuccessDir = OutDir + "/success";
  std::string FailureDir = OutDir + "/failure";
  mkdir(SuccessDir.c_str(), 0755);
  mkdir(FailureDir.c_str(), 0755);
}

std::string readOneFile(std::string &Path) {
  std::ifstream SeedFile(Path);
  std::string Line((std::istreambuf_iterator<char>(SeedFile)),
                   std::istreambuf_iterator<char>());
  return Line;
}

int readSeedInputs(std::vector<std::string> &SeedInputs,
                   std::string &SeedInputDir) {
  DIR *Directory;
  struct dirent *Ent;
  if ((Directory = opendir(SeedInputDir.c_str())) != NULL) {
    while ((Ent = readdir(Directory)) != NULL) {
      if (!(Ent->d_type == DT_REG))
        continue;
      std::string Path = SeedInputDir + "/" + std::string(Ent->d_name);
      std::string Line = readOneFile(Path);
      SeedInputs.push_back(Line);
    }
    closedir(Directory);
    return 0;
  } else {
    return 1;
  }
}

void readCoverageFile(std::string &Target,
                      std::vector<std::string> &CoverageData) {
  std::string CoveragePath = Target + ".cov";
  std::ifstream InFile(CoveragePath);
  std::string Line;
  while (std::getline(InFile, Line)) {
    CoverageData.push_back(Line);
  }
}

void storeSeed(std::string &OutDir, int randomSeed) {
  std::string Path = OutDir + "/randomSeed.txt";
  std::fstream File(Path, std::fstream::out | std::ios_base::trunc);
  File << std::to_string(randomSeed);
  File.close();
}

void storePassingInput(std::string &Input, std::string &OutDir) {
  std::string Path = OutDir + "/success/input" + std::to_string(successCount++);
  std::ofstream OutFile(Path);
  OutFile << Input;
  OutFile.close();
}

void storeCrashingInput(std::string &Input, std::string &OutDir) {
  std::string Path = OutDir + "/failure/input" + std::to_string(failureCount++);
  std::ofstream OutFile(Path);
  OutFile << Input;
  OutFile.close();
}

int runTarget(std::string &Target, std::string &Input) {
  std::string Cmd = Target + " > /dev/null 2>&1";
  FILE *F = popen(Cmd.c_str(), "w");
  fprintf(F, "%s", Input.c_str());
  return pclose(F);
}