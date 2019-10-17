############################################################
#
#   Compute ROC curves and AUC for Tadarida ID scores
#
#   DD4WH 15-09-2019
#
#
############################################################

library(data.table)
#library(plotROC) # this library does not work!
library(pROC)
library(ggplot2)

setwd("e:/Montpellier/Crex/R_scripts/2019_09_16_Compare_Classifiers")

ManIDtable=fread(paste0(getwd(),"/Classifier_comparison_7.csv"), dec=",", h=T)
str(ManIDtable)

summary(ManIDtable$IDscoreClassif2)

ManIDtable
ManIDtable$MANUAL_ID
ManIDtable$IDscoreClassif1

# calculating ROC cure, AUC, more options, CI and plotting
roc1 <- roc(ManIDtable$MANUAL_ID,
            ManIDtable$IDscoreClassif1, percent=TRUE,
            # arguments for auc
            #partial.auc=c(100, 90), partial.auc.correct=TRUE,
            #partial.auc.focus="sens",
            lwd = 5, cex.lab = 2.1, cex.axis = 1.4,
            # arguments for ci
            ci=F, boot.n=100, ci.alpha=0.9, stratified=FALSE,
            # arguments for plot
            plot=TRUE, auc.polygon=TRUE, max.auc.polygon=TRUE, grid=TRUE,
            #print.thres="local maximas", print.thres.pattern.cex = 2.0, print.thres.pch = 3,print.thres.col =2,
            #print.thres="best", print.thres.pattern.cex = 2.0, print.thres.pch = 3,print.thres.col =2,
            #print.thres=c(0.2, 0.4, 0.5, 0.6), print.thres.pattern.cex = 2.0, print.thres.pch = 3, print.thres.col =2,
            print.auc=TRUE, print.auc.x=36, print.auc.y=56, col = "Black", print.auc.cex = 2.0)

# Add to an existing plot. Beware of 'percent' specification!
roc2 <- roc(ManIDtable$MANUAL_ID,
            ManIDtable$IDscoreClassif2,
            ci=F, boot.n=100, ci.alpha=0.9, stratified=FALSE,
            lwd = 5,
            plot=TRUE, add=TRUE, percent=roc1$percent,
            #print.thres="best", print.thres.pattern.cex = 2.0, print.thres.pch = 3,print.thres.col =4,
            #print.thres=c(0.2, 0.4, 0.5, 0.6), print.thres.pattern.cex = 2.0, print.thres.pch = 3, print.thres.col =4,
            col = 3, print.auc=TRUE, print.auc.x=36, print.auc.y=48, print.auc.cex = 2.0)

roc3 <- roc(ManIDtable$MANUAL_ID,
            ManIDtable$IDscoreClassif3,
            ci=F, boot.n=100, ci.alpha=0.9, stratified=FALSE,
            lwd = 5,
            plot=TRUE, add=TRUE, percent=roc1$percent,
            #print.thres="best", print.thres.pattern.cex = 2.0, print.thres.pch = 3,print.thres.col =4,
            #print.thres=c(0.2, 0.4, 0.5, 0.6), print.thres.pattern.cex = 2.0, print.thres.pch = 3, print.thres.col =4,
            col = "Blue", print.auc=TRUE, print.auc.x=36, print.auc.y=40, print.auc.cex = 2.0)

roc4 <- roc(ManIDtable$MANUAL_ID,
            ManIDtable$IDscoreClassif4,
            ci=F, boot.n=100, ci.alpha=0.9, stratified=FALSE,
            lwd = 7,
            plot=TRUE, add=TRUE, percent=roc1$percent,
            #print.thres="best", print.thres.pattern.cex = 2.0, print.thres.pch = 3,print.thres.col =4,
            #print.thres=c(0.2, 0.4, 0.5, 0.6), print.thres.pattern.cex = 2.0, print.thres.pch = 3, print.thres.col =4,
            col = "Red", print.auc=TRUE, print.auc.x=36, print.auc.y=32, print.auc.cex = 2.0)

roc5 <- roc(ManIDtable$MANUAL_ID,
            ManIDtable$IDscoreClassif5,
            ci=F, boot.n=100, ci.alpha=0.9, stratified=FALSE,
            lwd = 5,
            plot=TRUE, add=TRUE, percent=roc1$percent,
            #print.thres="best", print.thres.pattern.cex = 2.0, print.thres.pch = 3,print.thres.col =4,
            #print.thres=c(0.2, 0.4, 0.5, 0.6), print.thres.pattern.cex = 2.0, print.thres.pch = 3, print.thres.col =4,
            col = "Darkgreen", print.auc=TRUE, print.auc.x=36, print.auc.y=24, print.auc.cex = 2.0)

roc6 <- roc(ManIDtable$MANUAL_ID,
            ManIDtable$IDscoreClassif6,
            ci=F, boot.n=100, ci.alpha=0.9, stratified=FALSE,
            lwd = 5,
            plot=TRUE, add=TRUE, percent=roc1$percent,
            #print.thres="best", print.thres.pattern.cex = 2.0, print.thres.pch = 3,print.thres.col =4,
            #print.thres=c(0.2, 0.4, 0.5, 0.6), print.thres.pattern.cex = 2.0, print.thres.pch = 3, print.thres.col =4,
            col = "Purple", print.auc=TRUE, print.auc.x=36, print.auc.y=16, print.auc.cex = 2.0)

roc7 <- roc(ManIDtable$MANUAL_ID,
            ManIDtable$IDscoreClassif7,
            ci=F, boot.n=100, ci.alpha=0.9, stratified=FALSE,
            lwd = 7,
            plot=TRUE, add=TRUE, percent=roc1$percent,
            #print.thres="best", print.thres.pattern.cex = 2.0, print.thres.pch = 3,print.thres.col =4,
            #print.thres=c(0.2, 0.4, 0.5, 0.6), print.thres.pattern.cex = 2.0, print.thres.pch = 3, print.thres.col =4,
            col = "Orange", print.auc=TRUE, print.auc.x=36, print.auc.y=8, print.auc.cex = 2.0)

(auc1 = auc(roc1))
(auc2 = auc(roc2))
(auc3 = auc(roc3))
(auc4 = auc(roc4))
(auc5 = auc(roc5))

coords(roc1, "best", ret=c("threshold", "specificity", "1-npv"))
coords(roc2, "best", ret=c("threshold", "specificity", "1-npv"))


# logistic regression
fit_glm <- glm(Crex ~ Idscore, data = DataM1, family=binomial(link="logit"))



###### from ROC example #######

# s100b --> Idscore
# outcome --> Crex --> 0/1

coords(roc2, "local maximas", ret=c("threshold", "sens", "spec", "ppv", "npv"))

## Confidence intervals ##

# CI of the AUC
ci(roc2)

## Not run: 
# CI of the curve
sens.ci <- ci.se(roc1, specificities=seq(0, 100, 5))
plot(sens.ci, type="shape", col="lightblue")
plot(sens.ci, type="bars")

## End(Not run)

# need to re-add roc2 over the shape
plot(roc2, add=TRUE)

## Not run: 
# CI of thresholds
plot(ci.thresholds(roc2))

## End(Not run)



# logistic regression
fit_glm <- glm(Crex ~ Idscore, data = DataM1, family=binomial(link="logit"))





