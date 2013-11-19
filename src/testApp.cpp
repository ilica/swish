#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    ofSetFrameRate(30);
    
    //Initialize the training and info variables
    infoText = "";
    trainingClassLabel = 1;
    record = false;
    
    //The input to the training data will be the [x y] from the mouse, so we set the number of dimensions to 2
    trainingData.setNumDimensions( 3 );
    
    
    //Initialize the DTW classifier
    DTW dtw;
    
    //Turn on null rejection, this lets the classifier output the predicted class label of 0 when the likelihood of a gesture is low
    dtw.enableNullRejection( true );
    
    //Set the null rejection coefficient to 3, this controls the thresholds for the automatic null rejection
    //You can increase this value if you find that your real-time gestures are not being recognized
    //If you are getting too many false positives then you should decrease this value
    dtw.setNullRejectionCoeff( 3 );
    
    //Turn on the automatic data triming, this will remove any sections of none movement from the start and end of the training samples
    dtw.enableTrimTrainingData(true, 0.1, 90);
    
    //Offset the timeseries data by the first sample, this makes your gestures (more) invariant to the location the gesture is performed
    dtw.setOffsetTimeseriesUsingFirstSample(true);
    
    //Add the classifier to the pipeline (after we do this, we don't need the DTW classifier anymore)
    pipeline.setClassifier( dtw );
}

//--------------------------------------------------------------
void testApp::update(){
    
    //Grab the current mouse x and y position
    VectorDouble sample(2);
    sample[0] = mouseX;
    sample[1] = mouseY;
    
    //If we are recording training data, then add the current sample to the training data set
    if( record ){
        timeseries.push_back( sample );
    }
    
    //If the pipeline has been trained, then run the prediction
    if( pipeline.getTrained() ){
        pipeline.predict( sample );
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofBackground(0, 0, 0);
    
    string text;
    int textX = 20;
    int textY = 20;
    
    //Draw the training info
    ofSetColor(255, 255, 255);
    text = "------------------- TrainingInfo -------------------";
    ofDrawBitmapString(text, textX,textY);
    
    if( record ) ofSetColor(255, 0, 0);
    else ofSetColor(255, 255, 255);
    textY += 15;
    text = record ? "RECORDING" : "Not Recording";
    ofDrawBitmapString(text, textX,textY);
    
    ofSetColor(255, 255, 255);
    textY += 15;
    text = "TrainingClassLabel: " + ofToString(trainingClassLabel);
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text = "NumTrainingSamples: " + ofToString(trainingData.getNumSamples());
    ofDrawBitmapString(text, textX,textY);
    
    
    //Draw the prediction info
    textY += 30;
    text = "------------------- Prediction Info -------------------";
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text =  pipeline.getTrained() ? "Model Trained: YES" : "Model Trained: NO";
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text = "PredictedClassLabel: " + ofToString(pipeline.getPredictedClassLabel());
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text = "Likelihood: " + ofToString(pipeline.getMaximumLikelihood());
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text = "SampleRate: " + ofToString(ofGetFrameRate(),2);
    ofDrawBitmapString(text, textX,textY);
    
    
    //Draw the info text
    textY += 30;
    text = "InfoText: " + infoText;
    ofDrawBitmapString(text, textX,textY);
    
    //Draw the timeseries data
    if( record ){
        ofFill();
        for(UINT i=0; i<timeseries.getNumRows(); i++){
            double x = timeseries[i][0];
            double y = timeseries[i][1];
            double r = ofMap(i,0,timeseries.getNumRows(),0,255);
            double g = 0;
            double b = 255-r;
            
            ofSetColor(r,g,b);
            ofEllipse(x,y,5,5);
        }
    }
    
    if( pipeline.getTrained() ){
        
        //Draw the data in the DTW input buffer
        DTW *dtw = pipeline.getClassifier< DTW >();
        
        if( dtw != NULL ){
            vector< VectorDouble > inputData = dtw->getInputDataBuffer();
            for(UINT i=0; i<inputData.size(); i++){
                double x = inputData[i][0];
                double y = inputData[i][1];
                double r = ofMap(i,0,inputData.size(),0,255);
                double g = 0;
                double b = 255-r;
                
                ofSetColor(r,g,b);
                ofEllipse(x,y,5,5);
            }
        }
    }
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    infoText = "";
    
    switch ( key) {
        case 'r':
            record = !record;
            if( !record ){
                trainingData.addSample(trainingClassLabel, timeseries);
                
                //Clear the timeseries for the next recording
                timeseries.clear();
            }
            break;
        case '[':
            if( trainingClassLabel > 1 )
                trainingClassLabel--;
            break;
        case ']':
            trainingClassLabel++;
            break;
        case 't':
            if( pipeline.train( trainingData ) ){
                infoText = "Pipeline Trained";
            }else infoText = "WARNING: Failed to train pipeline";
            break;
        case 's':
            if( trainingData.saveDatasetToFile("TrainingData.txt") ){
                infoText = "Training data saved to file";
            }else infoText = "WARNING: Failed to save training data to file";
            break;
        case 'l':
            if( trainingData.loadDatasetFromFile("TrainingData.txt") ){
                infoText = "Training data saved to file";
            }else infoText = "WARNING: Failed to load training data from file";
            break;
        case 'c':
            trainingData.clear();
            infoText = "Training data cleared";
            break;
        default:
            break;
    }
    
}