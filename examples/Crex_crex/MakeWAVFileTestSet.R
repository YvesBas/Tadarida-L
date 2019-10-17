#####################################################################################
#
#   Sample test set
#   SamplingRecordingsCrex.R
#
#   INPUT:    a lot of (eg. 100000) WAV files in subdirectories
#
#   OUTPUT:   a random sample of these WAV files copied into a new directory into subdirectories
#
#   
#####################################################################################

setwd("e:/Montpellier/Crex/")

# we have 96846 WAV files here
FolderW="n:/00_Audio/Wachtelkoenig_Projekt/2019/subset with ta files"

# make a list of these files
ListW=list.files(FolderW,pattern=".wav$",full.names=T,recursive=T)
WAVarray=as.matrix(ListW)
str(WAVarray)

# how many files to be randomly selected ?
NFiles=2500

# where should the sampled WAV files be copied?
NewDir=paste0(getwd(),"/WAVFileTestSet")

# randomly sample files from the list
SampleList=WAVarray[sample.int(nrow(WAVarray),NFiles),]

# make a list of all files that were sampled 
#PathSel=subset(ListW,basename(ListW) %in% as.list(SampleList))
# a list of all the subdirectories
#SubDirs=basename(dirname(PathSel))
SubDirs=basename(dirname(SampleList))
SubDirsU=unique(SubDirs)

# create the directory where the sampled WAV files will be copied to
dir.create(NewDir)
# create subdirectories
NewSubDir=paste0(NewDir,"/",SubDirsU)
for (i in 1:length(NewSubDir))
{
  dir.create(NewSubDir[i])
}

NewNames=paste0(NewDir,"/",SubDirs,"/",basename(SampleList))

file.copy(from=SampleList,to=NewNames)

