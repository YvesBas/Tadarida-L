###########################################################################
#
#   TADARIDA -  a toolbox for Animal Detection on Acoustic Recordings
#
#   Bas, Y, D Bas & J-F Julien (2017): Journal of Open Research Software 5: 6. 
#                                      DOI: https://doi.org/10.5334/jors.154  
#
#   Ta_Tc.R
#
#   Classify .ta-files with a readily built Classifier (*.learner)
# 
#   Inputs:   paths for scripts ClassifC1.R  AggContacts.R    AggNbSp.R
#             .ta-files of the WAV that shall be classified
#             Classifier (eg. ClassifEspFrance2019-09-14.learner)
#             SpeciesList.csv
#
#   Outputs:  ID scores for species in the Classifier for each WAV-file
#             --> in the file IDTotTot.csv
#
###########################################################################

# this is the folder where you will find the ID end results
setwd("e:/Montpellier/Crex/R_scripts/2019_09_17_Build_Crex_Classifier_7")

# specify the paths of these necessary R script files here
ClassifC1= paste0(getwd(),"/ClassifC1.R")
AggContacts=paste0(getwd(),"/AggContacts.R")
AggNbSp=paste0(getwd(),"/AggNbSp.R")

# name of the output file with ALL the IDs plus date
TotalFileName = paste0("/IdTotalTotal_Classifier7_test",substr(Sys.time(),1,10),".csv")


# just used to initialize args[], no need to change this
args="h:/Crex_crex"

# the path of a folder containing .ta files (Tadarida-D/L outputs)
# args[1] is not used any more! Use "tafolder" instead! DD4WH, 13-09-2019
# if you have ONE folder you want to use, uncomment the following:
#tafolder="e:/Montpellier/Crex/R_scripts/2019_09_10_Build_Crex_Classifier/ta_files/txt"

# if you have several folders where your .ta-files are, uncomment this:
dirlist=list.dirs("e:/Montpellier/Crex/R_scripts/2019_09_16_Compare_Classifiers/FileSelected_2500",recursive=T,full.names=T)
txtlist=subset(dirlist,grepl("/txt",dirlist))
tafolder=txtlist

# IMPORTANT:
# because IDTotal2019-09-14_x.csv files are appended, you have to make sure
# there are no "IdTotalx.csv"-files in the txt-subfolders from former classification approaches
# if former files are from another day, it is OK, because the filename contains the date.

# the path of the classifier to be used
#args[2]="ClassifEsp_LF_180320.learner"
#args[2]="ClassifEsp_LF_180129.learner"
#args[2]="ClassifEspFrance180303.learner"
args[2]= "ClassifEspFrance2019-09-17no_7.learner"

# (optional, and still undocumented) the path of a reference file giving the rank scale to convert features in linear discriminants
#args[3]="tabase3_LFXC"
args[3]="N"

#options (HPF = filtre passe-haut / Reduc = réduction des features par DFA)
# the high pass filter in kHz to filter out low frequency signals
#args[4]=8 #HPF 8kHz
args[4]=0 # no HPF

#args[5]=F #Reduc - obsolete

# Should outputs be splitted in .tc files (1 file per wave file) 
# or not (in this latter case outputs are written in one single table 
# IdTot.csv and shorter summary IdShort.csv AND in one table containing
# every ID score: IdTotalTotal.csv)
#args[6]=T #TC
args[6]=F #TC

# block size (number of .ta files to be classified at the same time - save memory down to 100)
args[7]=200 #block size

# the Nth file to start with
# args[8]

# the Nth to end with
# args[9]

# the path of a reference table giving the list of species codes
args[10]=paste0(getwd(),"/SpeciesList.csv") #species list

# the path of the boolean classifier 
# which will decide if a calls group is an additional species or not (output of buildClassifNbSp.r)
#args[11]="CNS_tabase3HF_France_IdConc.learner" #name of the species number" classifier

# if calls group should be pooled or not
args[12]=F #if species number should be filtered or not

# not yet documented
#args[13]="Referentiel_seuils_ProbEspHF_.csv"
args[14]=1e9 #an additionnal, larger block size to handle memory leaks problem in randomForest
args[15]=1 #block number 

# leave this as it is
args[16]=F #feed skip variable

# delete all previously present files that are supposed to be appended in this script
# only files from the same day (Sys.time) are removed!
for(a in 1:length(tafolder))
{
  tadir=tafolder[a]
  #if exists(fread(paste0(tadir,"/IdTotal",substr(Sys.time(),1,10),"_",a,".csv"))==T)
            file.remove(paste0(tadir,"/IdTotal",substr(Sys.time(),1,10),"_",a,".csv"))
}
file.remove(paste0(getwd(),TotalFileName))


print(getwd())
print(args)
# initialize dataframe for all the ID scores
IdTotTot=data.frame()
for(a in 1:length(tafolder))
{
  tadir=tafolder[a]
  
  talistot=list.files(tadir,pattern=".ta$",full.names=T)
  if(length(talistot)>as.numeric(args[14])*(as.numeric(args[15])-1))
  {
    talistot=talistot[(as.numeric(args[14])*(as.numeric(args[15])-1)+1)
                      :(min(length(talistot)
                            ,as.numeric(args[14])*(as.numeric(args[15]))))]
    
    # this single call needs a rather long time DD4WH
    FITA=file.info(talistot)
    talistot=subset(talistot,FITA$size>1000)

    block=as.numeric(args[7])
    
    Sys.time()
    for (r in 1:ceiling(length(talistot)/block))
    {
      args[8]=block*(r-1)+1 #start number
      args[9]=block*r #end number
      source(ClassifC1)
      print(paste(r,ceiling(length(talistot)/block),Sys.time()))
      if(!skip){
        source(AggContacts)
        source(AggNbSp)
        # write one block and append to the file
        # DD4WH 12-09-2019 
        fwrite(IdTot4,paste0(tadir,"/IdTotal",substr(Sys.time(),1,10),"_",a,".csv"), append = T)
      }
      
      print(gc())
      
    }
  }
  # DD4WH 12-09-2019 
  # write results for ALL folders
  # get results for all ta.-files in the current folder
  IdTotTot=fread(paste0(tadir,"/IdTotal",substr(Sys.time(),1,10),"_",a,".csv"))
  # append them to the total ID results file, which is located in the working directory
  write_directory = getwd()
  fwrite(IdTotTot,paste0(write_directory,TotalFileName), append = T)
}
Sys.time()

############# end of script ##############

# this is the output file of the Classifying process with all the species ID scores
AllIDs=fread(paste0(write_directory,TotalFileName))

# write simple output for Crex crex, just WAV-name and IDscore of Crex
# aggregate IDscores for the same WAV file (caused by two species identified in the same file)
# and take the maximum IDscore for that WAV file
CrexIDscores=aggregate(AllIDs$Crecre,by=list(AllIDs$Group.1),FUN=max)

colnames(CrexIDscores)[colnames(CrexIDscores)=="x"] <- paste0("CrexIDscore_no_7",substr(args[2],1,26))
colnames(CrexIDscores)

# write the file to csv with ID of the Classifier used
fwrite(CrexIDscores,paste0(write_directory,"/CrexIDscore_no_7",substr(args[2],1,26),".csv"))

######### end of Crex output writing ########

###############
# first look at the results DD4WH 13-09-2019 
###############

IdTotTot=fread(paste0(write_directory,TotalFileName))
str(IdTotTot)
# summary of the IDs with highest probabilities for the files
table(IdTotTot$SpMaxF2)

# Summary statistics for detected probabilites for Crex crex 
summary(IdTotTot$Crecre)

hist(IdTotTot$Crecre)

# how to calculate/aggregate probabilities for a species on a site?



