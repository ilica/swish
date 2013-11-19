#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    ofSetFrameRate(30);
    
    infoText = "";
    trainingClassLabel = 1;
    record = false;
    UINT RIGHT_LABEL = 1;
    UINT LEFT_LABEL = 2;
    
    trainingData.setNumDimensions( 3 );
    
    DTW dtw;
    
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
    
    
    // The follwing 4 steps must be done for each training swipe, which constists of a series of (x,y,z)'s
    // These sould probably be loaded in from a file
    
    // STEP 1: LOAD TRAINING DATA HERE! this is just fake sample data
    // 0 -> x, 1 -> y, 3 -> z
    VectorDouble rightSwipeMovement1(3);
    rightSwipeMovement1[0] = 39;
    rightSwipeMovement1[1] = -3; // < 0 so moving right
    rightSwipeMovement1[2] = 188;
    VectorDouble rightSwipeMovement2(3);
    rightSwipeMovement1[0] = 41;
    rightSwipeMovement1[1] = -5; // < 0 so moving right
    rightSwipeMovement1[2] = 121;
    VectorDouble rightSwipeMovement3(3);
    rightSwipeMovement1[0] = 22;
    rightSwipeMovement1[1] = -20; // < 0 so moving right
    rightSwipeMovement1[2] = 188;
    VectorDouble rightSwipeMovement4(3);
    rightSwipeMovement1[0] = 39;
    rightSwipeMovement1[1] = 3; // > 0 so finished and moving back to the left
    rightSwipeMovement1[2] = 108;
    
    // STEP 2: add this series of movements onto the timeseries
    timeseries.push_back(rightSwipeMovement1);
    timeseries.push_back(rightSwipeMovement2);
    timeseries.push_back(rightSwipeMovement3);
    timeseries.push_back(rightSwipeMovement4);
    
    // STEP 3: Add this as training data with the correct label
    trainingData.addSample(RIGHT_LABEL, timeseries);
    // STEP 4: Clear the timeseries so the next things you add arent part of the same motion
    timeseries.clear();
    
    
    // Now once we have added enough training data by doing these steps for many examples, train it
    pipeline.train( trainingData );
}

//--------------------------------------------------------------
void testApp::update(){
    ///////////////////////
    // this is where we want to pull the latest value from the accelerometer
    // (not just add 0's obv)
    ///////////////////////
    
    VectorDouble sample(3);
    sample[0] = 0;
    sample[1] = 0;
    sample[2] = 0;
    
    if( record ){
        timeseries.push_back( sample );
    }
    
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
    
    
    textY += 30;
    text = "------------------- Prediction Info -------------------";
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text =  pipeline.getTrained() ? "Model Trained: YES" : "Model Trained: NO";
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    
    // this always outputs the class label on the screen, using the latest value from
    // the data added in the update method
    text = "PredictedClassLabel: " + ofToString(pipeline.getPredictedClassLabel());
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text = "Likelihood: " + ofToString(pipeline.getMaximumLikelihood());
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text = "SampleRate: " + ofToString(ofGetFrameRate(),2);
    ofDrawBitmapString(text, textX,textY);
    
    
    textY += 30;
    text = "InfoText: " + infoText;
    ofDrawBitmapString(text, textX,textY);
    
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