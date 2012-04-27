#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	ofBackground(0);

#ifdef TARGET_WIN32
	pathDelim = "\\";
#else
	pathDelim = "/";
#endif
    pauseRender = false;
    
    cam.setup();
	cam.speed = 40;
	cam.autosavePosition = true;
	cam.usemouse = true;
	cam.useArrowKeys = false;
	cam.setFarClip(30000);
	cam.setScale(1, -1, 1);
	cam.targetNode.setScale(1,-1,1);
	cameraTrack.setCamera(cam);
	cam.loadCameraPosition();
	cam.maximumY =  120;
	cam.minimumY = -120;
	cam.rollSpeed = 1;
    
    currentMirror = false;
	currentSimplify = 2;
	
//	farClip = 5000;
	videoInPercent = 0.0;
	videoOutPercent = 1.0;
	enableVideoInOut = false;
	
	currentZFuzz = 0;
	
	hiResPlayer = NULL;
	lowResPlayer = NULL;
	startRenderMode = false;
	currentlyRendering = false;
	allLoaded = false;
	playerElementAdded = false;
	presentMode = false;
	viewComps = false;
	
	shouldResetDuration = false;
    setDurationToClipLength = false;

    drawDepthDistortion = true;
	drawGeometryDistortion = true;

	shouldSaveCameraPoint = false;
	shouldClearCameraMoves = false;
	
	temporalAlignmentMode = false;
	captureFramePair = false;

	fillHoles = false;
	currentHoleKernelSize = 1;
	currentHoleFillIterations = 1;
	setDurationToClipLength = false;
    
	sampleCamera = false;
	
	savingImage.setUseTexture(false);
	savingImage.allocate(1920,1080, OF_IMAGE_COLOR);
	
	fboRectangle = ofRectangle(250, 100, 1280*.75, 720*.75);
    swapFbo.allocate(1920, 1080, GL_RGBA, 4);
	curbuf = 0;
    fbo1.allocate(1920, 1080, GL_RGBA32F_ARB);
	fbo2.allocate(1920, 1080, GL_RGBA32F_ARB);
    
    fbo1.begin();
    ofClear(0,0,0,0);
    fbo1.end();
    fbo2.begin();
    ofClear(0,0,0,0);
    fbo2.end();
    
    DOFCloud.load("shaders/DOFCloud");
    DOFCloud.begin();
    DOFCloud.setUniform1i("src_tex_unit0", 0);
    DOFCloud.end();
 
    alphaFadeShader.load("shaders/alphafade");
    alphaFadeShader.begin();
    alphaFadeShader.setUniform1i("self", 0);
    alphaFadeShader.end();

    gaussianBlur.load("shaders/gaussian_blur");
    gaussianBlur.begin();
    gaussianBlur.setUniform1i("self", 0);
    gaussianBlur.end();
	
	meshOccludeShader.load("shaders/meshOcclude");
	
	dynamicLuminosityShader.load("shaders/dynamicLuminosity");
	dynamicLuminosityShader.begin();
	dynamicLuminosityShader.setUniform1i("self",0);
	dynamicLuminosityShader.end();
	
	/*perlinDensityShader.load("shaders/perlinluminosityblend");
	perlinDensityShader.begin();
	perlinDensityShader.setUniform1i("self",0);
	perlinDensityShader.end();*/

	newCompButton = new ofxMSAInteractiveObjectWithDelegate();
	newCompButton->setLabel("New Comp");
	newCompButton->setDelegate(this);
	newCompButton->setPosAndSize(fboRectangle.x+fboRectangle.width+25, 0, 100, 25);
	
	saveCompButton = new ofxMSAInteractiveObjectWithDelegate();
	saveCompButton->setLabel("Save Comp");
	saveCompButton->setDelegate(this);
	saveCompButton->setPosAndSize(fboRectangle.x+fboRectangle.width+25, 25, 100, 25);

	timeline.setup();
	timeline.setMovePlayheadOnDrag(true);
	timeline.getColors().loadColors("defaultColors.xml");
	timeline.setOffset(ofVec2f(0, ofGetHeight() - 200));
	timeline.setPageName("Main");
	timeline.setDurationInFrames(300); //base duration
    timeline.setMovePlayheadOnDrag(false);
    
	loadCompositions();

	gui.addToggle("Pause Render", pauseRender);
	gui.addSlider("Simplify", currentSimplify, 1, 8);
	gui.addToggle("Draw Pointcloud", drawPointcloud);
	gui.addToggle("Draw Wireframe", drawWireframe);
	gui.addToggle("Draw Mesh", drawMesh);
	gui.addToggle("Depth Distortion", drawDepthDistortion);
	gui.addToggle("Geometry Distortion", drawGeometryDistortion);

	gui.addPage("Camera");
    gui.addToggle("Reset Cam Pos", shouldResetCamera);
	gui.addSlider("Camera Speed", cam.speed, .1, 40);
	gui.addSlider("Cam Roll Speed", cam.rollSpeed, .01, 2);
	gui.addToggle("Set Camera Point", shouldSaveCameraPoint);
	gui.addToggle("Lock to Track", currentLockCamera);
    
	gui.addPage("Duration");
	gui.addSlider("Duration", currentDuration, 2*30, 8*60*30);
	gui.addToggle("Set Duration", shouldResetDuration);
    gui.addToggle("Reset Duration to Clip", setDurationToClipLength);
	gui.addToggle("Video In/Out", enableVideoInOut);
	gui.addSlider("Video In", videoInPercent, 0, 1.0);
	gui.addSlider("Video Out", videoOutPercent, 0, 1.0);
	
	gui.addPage("Tweaks");
    gui.addToggle("Mirror", currentMirror);
	gui.addSlider("X Multiply Shift", currentXMultiplyShift, -75, 75);
	gui.addSlider("Y Multiply Shift", currentYMultiplyShift, -75, 75);
	gui.addToggle("Fill Holes", fillHoles);
	gui.addSlider("Hole Kernel Size", currentHoleKernelSize, 1, 20);
	gui.addSlider("Hole Fill Iterations", currentHoleFillIterations, 1, 20);
	gui.addSlider("Z Fuzz", currentZFuzz, 0, .25);
	gui.addToggle("TemporalAlignmentMode", temporalAlignmentMode);
	gui.addToggle("Capture Frame Pair", captureFramePair);

	gui.addPage("Batching");
	gui.addToggle("View Comps", viewComps);
	gui.addToggle("Render Batch", startRenderMode);

	gui.loadFromXML();
	gui.toggleDraw();
	
	currentXScale = 1.0;
	currentYScale = 1.0;
	currentXAdditiveShift = 0;
	currentYAdditiveShift = 0;
	currentRotationCompensation = 0;	
	
	
	currentLockCamera = false;
	cameraTrack.lockCameraToTrack = false;
    
}


//Labbers: YOU CAN ADD TIMELINE ELEMENTS HERE
void testApp::populateTimelineElements(){
	
	timeline.setPageName("Camera");
	timeline.addElement("Camera", &cameraTrack);
	timeline.addElement("Video", &videoTimelineElement);

    //rendering
    timeline.addPage("Global Rendering", true);
    timeline.addKeyframes("Edge Snip", currentCompositionDirectory + "edgeSnip.xml", ofRange(1.0, 6000), 6000 );
    timeline.addKeyframes("Z Threshold", currentCompositionDirectory + "zThreshold.xml", ofRange(1.0, 6000), 6000 );
    timeline.addKeyframes("Motion Trail Decay", currentCompositionDirectory + "motionTrailDecay.xml", ofRange(.05 ,1.0), 1.0 );
    
    timeline.addPage("Mesh", true);
    timeline.addKeyframes("Mesh Alpha", currentCompositionDirectory + "meshAlpha.xml", ofRange(0,1.0) );
	timeline.addKeyframes("X Rotate", currentCompositionDirectory + "meshXRot.xml", ofRange(-360,360), 0.);
    timeline.addKeyframes("Y Rotate", currentCompositionDirectory + "meshYRot.xml", ofRange(-360,360), 0.);
    timeline.addKeyframes("Z Rotate", currentCompositionDirectory + "meshZRot.xml", ofRange(-360,360), 0.);
    
    timeline.addPage("Wireframe", true);
    timeline.addKeyframes("Wireframe Alpha", currentCompositionDirectory + "wireframeAlpha.xml", ofRange(0,1.0), 1.0 );
    timeline.addKeyframes("Wireframe Thickness", currentCompositionDirectory + "wireframeThickness.xml", ofRange(1.0,20.0) );
    timeline.addKeyframes("Wireframe Blur", currentCompositionDirectory + "wireframeBlur.xml", ofRange(0, 5.0) );

    timeline.addPage("Point", true);
    timeline.addKeyframes("Point Alpha", currentCompositionDirectory + "pointAlpha.xml", ofRange(0,1.0) );
    timeline.addKeyframes("Point Size", currentCompositionDirectory + "pointSize.xml", ofRange(1.0,20.0) );	
    
    timeline.addPage("Point DOF", true);
    timeline.addKeyframes("Point DOF Size", currentCompositionDirectory + "dofPointSize.xml", ofRange(1.0, 20.0) );	
    timeline.addKeyframes("Point DOF Alpha", currentCompositionDirectory + "dofPointAlpha.xml", ofRange(0, 1.0) );
    timeline.addKeyframes("Point DOF Focal", currentCompositionDirectory + "dofPointFocalDistance.xml", ofRange(0, 600) );
    timeline.addKeyframes("Point DOF Aperture", currentCompositionDirectory + "dofPointAperture.xml", ofRange(0, .1) );
    timeline.addKeyframes("Point DOF Reduce Blowout", currentCompositionDirectory + "dofBlowoutReduce.xml", ofRange(1.0, 20.0) );
    
    timeline.addPage("Depth Distortion", true);
    timeline.addKeyframes("Noise", currentCompositionDirectory + "DepthNoise.xml", ofRange(0, 5000) );
    timeline.addKeyframes("Sine Amplitude", currentCompositionDirectory + "SineAmp.xml", ofRange(0, 15) );
    timeline.addKeyframes("Sine Speed", currentCompositionDirectory + "SineSpeed.xml", ofRange(-200, 200) );
    timeline.addKeyframes("Sine Period", currentCompositionDirectory + "SinePeriod.xml", ofRange(.01, 10) );
    
    timeline.addPage("Geometry  Distortion", true);
    timeline.addKeyframes("Perlin Amp", "PerlinAmp.xml", ofRange(0, 200.0) );
    timeline.addKeyframes("Perlin Density", "PerlinDensity.xml", ofRange(0, 200.0) );
    timeline.addKeyframes("Perlin Speed", "PerlinSpeed.xml", ofRange(0, 200.0) );    
    
//    timeline.addPage("Lumen Noise", true);
    
	timeline.addPage("Alignment", true);
	timeline.addElement("Video", &videoTimelineElement);
	timeline.addElement("Depth Sequence", &depthSequence);
	timeline.addElement("Alignment", &alignmentScrubber);
    
	timeline.addPage("Dynamic Luminosity", true);
	timeline.addKeyframes("Black Fade", currentCompositionDirectory + "dlBlackFade.xml",ofRange(0.0,1.0));
	timeline.addKeyframes("Luminosity Effect", currentCompositionDirectory + "dlLuminosityEffect.xml",ofRange(0.0,1.0));
	timeline.addKeyframes("Luminosity Time Scale", currentCompositionDirectory + "dlTimeScale.xml", ofRange(-1.0,1.0));
	timeline.addKeyframes("Luminosity Perlin Range", currentCompositionDirectory + "dlPerlinRange.xml", ofRange(0.0,1.0));
	timeline.addKeyframes("Luminosity Perlin Amount", currentCompositionDirectory + "dlPerlinAmt.xml", ofRange(0.0,1.0));
	timeline.addKeyframes("Luminosity Sine Period",currentCompositionDirectory+ "dlSinePeriod.xml", ofRange(0.01,10.0));
	timeline.addKeyframes("Luminosity Sine Amount",currentCompositionDirectory+ "dlSineAmount.xml", ofRange(0.0,1.0));
/*	timeline.addKeyrames("Red Fade", currentCompositionDirectory + "dlRedFade.xml",ofRange(0.0,1.0));
	timeline.addKeyframes("Blue Fade", currentCompositionDirectory + "dlBlueFade.xml",ofRange(0.0,1.0));
	timeline.addKeyframes("Green Fade", currentCompositionDirectory + "dlGreenFade.xml",ofRange(0.0,1.0));*/

	
	timeline.setCurrentPage("Rendering");
	
	playerElementAdded = true;
}

#pragma mark customization
//--------------------------------------------------------------
void testApp::processDepthFrame(){
	
    if(!drawDepthDistortion) return;
    
    float noise = timeline.getKeyframeValue("Noise");
    float sineAmp = timeline.getKeyframeValue("Sine Amplitude");
    sineAmp *= sineAmp;
    float sineSpeed = timeline.getKeyframeValue("Sine Speed");
    float sinePeriod = timeline.getKeyframeValue("Sine Period");
    
	for(int y = 0; y <	480; y++){
		for(int x = 0; x < 640; x++){
			int index = y*640+x;
            
			//***************************************************
			//CUSTOMIZATION: YOU CAN PROCESS YOUR RAW DEPTH FRAME HERE
			//* 
			//* depthPixelDecodeBuffer contains the raw depth image
			//*
			//***************************************************			
            if(noise > 0){
                holeFilledPixels.getPixels()[index] += ofRandom(noise);   
            }
            
            if(sineAmp > 0){
                holeFilledPixels.getPixels()[index] += sin( y * sinePeriod + timeline.getCurrentFrame() * sineSpeed ) * sineAmp;
            }
            			
			//for example delete every other line
//			if(y % 4 == 0){
//				depthPixelDecodeBuffer[index] = 0;
//			}
		}
	}
}

void testApp::processGeometry(){
	//***************************************************
	//CUSTOMIZATION: YOU CAN MANIPULATE YOUR MESH HERE
	//*
	//* renderer.getMesh() contains mesh with indeces and texture coordinates 
	//*
	//***************************************************
    if(!drawGeometryDistortion) return;

    
    float perlinAmp = timeline.getKeyframeValue("Perlin Amp");
    float perlinDensity = timeline.getKeyframeValue("Perlin Density");
    float perlinSpeed = timeline.getKeyframeValue("Perlin Speed");
//    float contract = timeline.getKeyframeValue("Contract");
//    float explode = timeline.getKeyframeValue("Explode");
    float contract = 0; //timeline.getKeyframeValue("Contract");
    float explode = 0; //timeline.getKeyframeValue("Explode");
    float blackFade = timeline.getKeyframeValue("Black Fade");
	/*float redFade = timeline.getKeyframeValue("Red Fade");
	float blueFade = timeline.getKeyframeValue("Blue Fade");
	float greenFade = timeline.getKeyframeValue("Green Fade");*/
	
    ofVec3f center(0,0,0);
    for(int i = 0; i < renderer.getMesh().getVertices().size(); i++){
        center += renderer.getMesh().getVertex(i);
    }
    center /= renderer.getMesh().getVertices().size();
    
    for(int i = 0; i < renderer.getMesh().getVertices().size(); i++){
        ofVec3f& vert = renderer.getMesh().getVertices()[i];
        if(perlinAmp > 0){
            renderer.getMesh().getVertices()[i] += ofVec3f(ofSignedNoise(vert.x/perlinDensity, vert.y/perlinDensity, vert.z/perlinDensity, ofGetElapsedTimef()/perlinDensity)*perlinAmp,
                                                           ofSignedNoise(vert.z/perlinDensity, vert.x/perlinDensity, vert.y/perlinDensity, ofGetElapsedTimef()/perlinDensity)*perlinAmp,
                                                           ofSignedNoise(vert.y/perlinDensity, vert.z/perlinDensity, vert.x/perlinDensity, ofGetElapsedTimef()/perlinDensity)*perlinAmp );
        }
        
        //vert.interpolate(center, contract);
        if(explode != 0){
            vert += (vert - center).normalize() * explode;
        }
		renderer.getMesh().setColor(i,ofFloatColor(blackFade));
		//renderer.getMesh().setColor(i,ofFloatColor(ofSignedNoise(renderer.getMesh().getColor(i).r,renderer.getMesh().getColor(i).g,renderer.getMesh().getColor(i).b)));
		
    }
    
//	for(int i = 0; i < renderer.getMesh().getVertices().size(); i++){
//		renderer.getMesh().getVertices()[i].z += sin(i/30.0 + timeline.getCurrentFrame())*25;
//	}
	
	
}

void testApp::drawGeometry(){
	
	//***************************************************
	//CUSTOMIZATION: YOU CAN DRAW WHATEVER YOU WANT HERE TOO OR USE SHADERS
	//*
	//* draw whatever you want!
	//*
	//***************************************************
	
    float pointAlpha = timeline.getKeyframeValue("Point Alpha");
    float pointDOFAlpha = timeline.getKeyframeValue("Point DOF Alpha");
    float wireAlpha = timeline.getKeyframeValue("Wireframe Alpha");
    float meshAlpha = timeline.getKeyframeValue("Mesh Alpha");

    if(!alignmentScrubber.ready()){
        pointAlpha = 0;
        pointDOFAlpha = 0;
        wireAlpha = 1.0;
        meshAlpha = 0.0;    
    }
    
    //helps eliminate zfight by translating the mesh occluder slightly back from the camera
    ofVec3f camTranslateVec = cam.getLookAtDir();
    ofFbo& fbo = curbuf == 0 ? fbo1 : fbo2;
    ofFbo& backbuf = curbuf == 0 ? fbo2 : fbo1;
    //curbuf = (curbuf + 1 ) % 2;
    
    ofRectangle renderFboRect = ofRectangle(0, 0, fbo.getWidth(), fbo.getHeight());
    
    if(pauseRender){
        ofPushStyle();
        ofSetColor(255,0,0);
        
        ofRect(fboRectangle);
        fbo.getTextureReference().draw(fboRectangle);
        ofPopStyle();
	    return;
    }
    
    /*
     old alpha fade way not working
    fbo.begin();
    ofClear(0.0, 0.0, 0.0, 0.0);
    ofEnableAlphaBlending();
    alphaFadeShader.begin();
    float decay = timeline.getKeyframeValue("Motion Trail Decay");
    decay *= decay;
    
    alphaFadeShader.setUniform1f("fadeSpeed", decay);
    backbuf.getTextureReference().draw(renderFboRect);
    
	alphaFadeShader.end();
    
    fbo.end();
	*/
    
    //new way of doing trails that kills alpha backgrounds.
    fbo.begin();
    ofPushStyle();
    ofEnableAlphaBlending();
    float decay = timeline.getKeyframeValue("Motion Trail Decay");
    decay *= decay; //exponential
    ofSetColor(0, 0, 0, decay*255);
    ofRect(renderFboRect);
    ofPopStyle();
    fbo.end();


    if(drawMesh && meshAlpha > 0){
        
        swapFbo.begin();
        ofClear(0,0,0,0);
        
        cam.begin(renderFboRect);
        
        ofPushStyle();
		//perlinDensityShader.begin();
		//perlinDensityShader.setUniform1f("time",(float)timeline.getCurrentFrame());
		renderer.drawMesh();
		//perlinDensityShader.end();
		ofPopStyle();
        
        cam.end();
        
        swapFbo.end();

        //composite
        fbo.begin();
        
        ofPushStyle();
        ofEnableAlphaBlending();
        ofSetColor(255, 255, 255, meshAlpha*255);
        swapFbo.getTextureReference().draw(renderFboRect);
        ofPopStyle();
        
        fbo.end();        
	}
    
    if(drawWireframe && wireAlpha > 0){
        
        swapFbo.begin();
        ofClear(0,0,0,0);
        
        cam.begin(renderFboRect);
        
        ofPushStyle();
        
        //occlude points behind the mesh
        ofPushMatrix();
        ofEnableAlphaBlending();
        ofSetColor(0, 0, 0, 0);
        ofTranslate(camTranslateVec);
		meshOccludeShader.begin();
		//vector<ofFloatColor> meshColors = renderer.getMesh().getColors();
		//renderer.getMesh().clearColors();
        renderer.drawMesh();
		meshOccludeShader.end();
		//renderer.getMesh().addColors(meshColors);
        ofPopMatrix();

        
//        glEnable(GL_BLEND);
//        glBlendFunc(GL_ONE, GL_ZERO);
//        glDisable(GL_SMOOTH_LINE_WIDTH_GRANULARITY);
//        glDisable(GL_SMOOTH_LINE_WIDTH_RANGE);
//        ofDisableSmoothing();
//        ofDisableAlphaBlending();
		ofEnableAlphaBlending();
        
		ofSetColor(255);        
        ofSetLineWidth(timeline.getKeyframeValue("Wireframe Thickness"));
		dynamicLuminosityShader.begin();
		dynamicLuminosityShader.setUniform1f("luminosityEffect",timeline.getKeyframeValue("Luminosity Effect"));
		dynamicLuminosityShader.setUniform1f("time",(float)timeline.getCurrentFrame()*timeline.getKeyframeValue("Luminosity Time Scale"));
		dynamicLuminosityShader.setUniform1f("rangescale",timeline.getKeyframeValue("Luminosity Perlin Range"));
		dynamicLuminosityShader.setUniform1f("perlinAmount", timeline.getKeyframeValue("Luminosity Perlin Amount"));
		dynamicLuminosityShader.setUniform1f("sinePeriod",timeline.getKeyframeValue("Luminosity Sine Period"));
		dynamicLuminosityShader.setUniform1f("sineAmount",timeline.getKeyframeValue("Luminosity Sine Amount"));
		renderer.drawWireFrame();
		dynamicLuminosityShader.end();
		ofPopStyle();      
        cam.end();
        
        swapFbo.end();
	
        //composite
        fbo.begin();
        
        ofPushStyle();
        ofEnableAlphaBlending();
        ofSetColor(255, 255, 255, wireAlpha*255);
        float blur = timeline.getKeyframeValue("Wireframe Blur");
        if(blur > 0){
            gaussianBlur.begin();
            gaussianBlur.setUniform2f("sampleOffset", 0, blur);
            swapFbo.getTextureReference().draw(renderFboRect);
            
            gaussianBlur.setUniform2f("sampleOffset", blur, 0);
            swapFbo.getTextureReference().draw(renderFboRect);

            gaussianBlur.end();
        }
        else{
        	swapFbo.getTextureReference().draw(renderFboRect);
        }
        ofPopStyle();
        
        fbo.end();     
    }
    
    if(drawPointcloud){
        if(pointDOFAlpha > 0){
            
            swapFbo.begin();
            ofClear(0,0,0,0);
            
            cam.begin(renderFboRect);
                        
            ofPushStyle();
            glPushAttrib(GL_ENABLE_BIT);
            
            ofEnableBlendMode(OF_BLENDMODE_SCREEN);
            glEnable(GL_POINT_SMOOTH); // makes circular points
            glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);	// allows per-point size
            glDisable(GL_DEPTH_TEST);		
            
            DOFCloud.begin();
            DOFCloud.setUniform1f("focusDistance", timeline.getKeyframeValue("Point DOF Focal") );
            DOFCloud.setUniform1f("aperture", timeline.getKeyframeValue("Point DOF Aperture") );
            DOFCloud.setUniform1f("minPointSize", timeline.getKeyframeValue("Point DOF Size"));
            DOFCloud.setUniform1f("blowoutReduce", timeline.getKeyframeValue("Point DOF Reduce Blowout"));

            ofPushMatrix();
			ofScale(1, -1, 1);
            ofRotate(renderer.meshRotate.x,1,0,0);
            ofRotate(renderer.meshRotate.y,0,1,0);
            ofRotate(renderer.meshRotate.z,0,0,1);
            
            renderer.getRGBTexture().getTextureReference().bind();
            renderer.getMesh().drawVertices();        
            renderer.getRGBTexture().getTextureReference().unbind();
            ofPopMatrix();
            
            DOFCloud.end();
            
            glPopAttrib();
            ofPopStyle();
            
            cam.end();
            
            swapFbo.end();
            
            //composite
            fbo.begin();
            
            ofPushStyle();
            ofEnableAlphaBlending();
            ofSetColor(255, 255, 255, pointDOFAlpha*255);
            swapFbo.getTextureReference().draw(renderFboRect);
            ofPopStyle();
            
            fbo.end();
        }

        if(pointAlpha > 0){
            
            //render
            swapFbo.begin();
            ofClear(0,0,0,0);
            
            cam.begin(renderFboRect);
            
            ofPushStyle();
            //occlude points behind the mesh
            ofPushMatrix();
            ofEnableAlphaBlending();
            ofSetColor(0, 0, 0, 0);
            ofTranslate(camTranslateVec);
			meshOccludeShader.begin();
            renderer.drawMesh();
			meshOccludeShader.end();
            ofPopMatrix();
            
            ofSetColor(255);
            ofEnableAlphaBlending();
            glEnable(GL_POINT_SMOOTH); // makes circular points
            glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);	// allows per-point size
            glEnable(GL_BLEND);
            //glBlendFunc(GL_ZERO, GL_ONE);
            glPointSize(timeline.getKeyframeValue("Point Size"));
			//perlinDensityShader.begin();
			//perlinDensityShader.setUniform1f("time",(float)timeline.getCurrentFrame()*.01f);
            renderer.drawPointCloud();
		//	perlinDensityShader.end();
            ofPopStyle();
            
            cam.end();
            
            swapFbo.end();
            
            //composite
            fbo.begin();
            
            ofPushStyle();
            ofEnableAlphaBlending();
            ofSetColor(255, 255, 255, pointAlpha*255);
			/*perlinDensityShader.begin();
			perlinDensityShader.setUniformTexture("self",swapFbo.getTextureReference(),0);
			glBegin(GL_QUADS);
			glVertex2f(renderFboRect.x,renderFboRect.y);
			glMultiTexCoord2f(0,0.0,0.0);
			glVertex2f(renderFboRect.x+renderFboRect.width,renderFboRect.y);
			glMultiTexCoord2f(0,1.0,0.0);
			glVertex2f(renderFboRect.x+renderFboRect.width,renderFboRect.y+renderFboRect.height);
			glMultiTexCoord2f(0,1.0,1.0);
			glVertex2f(renderFboRect.x,renderFboRect.y+renderFboRect.height);
			glMultiTexCoord2f(0,0.0,1.0);
			glEnd();
			perlinDensityShader.end();*/
			swapFbo.getTextureReference().draw(renderFboRect);
            ofPopStyle();
            
            fbo.end();
            
        }
    }
    
    ofPushStyle();
    ofSetColor(0);
    ofRect(fboRectangle);
    ofPopStyle();
    
    ofEnableAlphaBlending();
    fbo.getTextureReference().draw(fboRectangle);
    
}

//************************************************************
///CUSTOMIZATION: Feel free to add things for interaction here
//************************************************************
//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
	if(key == 'f'){
		ofToggleFullscreen();
	}
	
	if(!allLoaded) return;
	
    if(key == 'P'){
    	pauseRender = !pauseRender;
    }
    
	if(currentlyRendering){
		if(key == ' '){
			finishRender();
		}
		return;
	}
	
	if(key == ' '){
		if(lowResPlayer->getSpeed() != 0.0){
			lowResPlayer->setSpeed(0);
//			stopCameraPlayback();
		}
		else{
			lowResPlayer->play();
			lowResPlayer->setSpeed(1.0);
		}		
	}
	
    if(key == 'T'){
    	shouldSaveCameraPoint = true;
    }
    
    if(key == 'L'){
    	currentLockCamera = !currentLockCamera;
    }
        
	if(key == 'i'){
		timeline.setCurrentTimeToInPoint();	
	}
	
	if(key == 'o'){
		timeline.setCurrentTimeToOutPoint();
	}
	

	//RECORD CAMERA
//	if(key == 'R'){	
//		toggleCameraRecord();
//	}
	
	//PLAYBACK CAMERA
//	if(key == 'P'){
//		toggleCameraPlayback();
//	}
	
	if(key == '\t'){
		videoTimelineElement.toggleThumbs();
		depthSequence.toggleThumbs();
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	cam.usemouse = fboRectangle.inside(x, y) && !pauseRender;
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	
}


//***************************************************
///Labbers: Everything below here is application logic
//***************************************************
#pragma mark application logic
//--------------------------------------------------------------
void testApp::update(){
	
	for(int i = 0; i < comps.size(); i++){
		comps[i]->load->enabled    = viewComps || !allLoaded;
		comps[i]->toggle->enabled  = viewComps || !allLoaded;
	}
	
	if(shouldResetDuration){
		timeline.setDurationInFrames(currentDuration);
		shouldResetDuration = false;
	}
	
	if(!allLoaded) return;
	
	//if we don't have good pairings, force pages on timeline + gui
	if(!alignmentScrubber.ready()){
		videoTimelineElement.setInFrame(0);
		videoTimelineElement.setOutFrame(lowResPlayer->getTotalNumFrames());
        temporalAlignmentMode = true;
        drawPointcloud = false;
        drawMesh = false;
        drawWireframe = true;
		timeline.setCurrentPage("Alignment");
//        gui.setPage(4);

	}
	
	if(currentLockCamera != cameraTrack.lockCameraToTrack){
		if(!currentLockCamera){

			cam.targetNode.setPosition(cam.getPosition());
			cam.targetNode.setOrientation(cam.getOrientationQuat());
			cam.rotationX = cam.targetXRot = -cam.getHeading();
			cam.rotationY = cam.targetYRot = -cam.getPitch();
			cam.rotationZ = -cam.getRoll();
		}
		cameraTrack.lockCameraToTrack = currentLockCamera;
	}
	
	if(cameraTrack.lockCameraToTrack){
		cameraTrack.setTimelineInOutToTrack();
	}
	else{
		timeline.setInOutRange(ofRange(0,1));
	}
	
	
	cam.applyRotation = !cameraTrack.lockCameraToTrack;
	cam.applyTranslation = !cameraTrack.lockCameraToTrack;

	if(enableVideoInOut){
		videoTimelineElement.setInFrame(videoInPercent*lowResPlayer->getTotalNumFrames());
		videoTimelineElement.setOutFrame(videoOutPercent*lowResPlayer->getTotalNumFrames());
	}
	else{
		videoTimelineElement.setInFrame(0);
		videoTimelineElement.setOutFrame(lowResPlayer->getTotalNumFrames());		
	}
	
	if(startRenderMode && !hasHiresVideo){
		ofSystemAlertDialog("This composition doesn't have a hi res movie so we can't render!");
		startRenderMode = false;
	}
	
	if(startRenderMode){
        fbo1.begin();
        ofClear(0,0,0,0);
        fbo1.end();
        fbo2.begin();
        ofClear(0,0,0,0);
        fbo2.end();
        
		viewComps = false;
		saveComposition();
		for(int i = 0; i < comps.size(); i++){
			if(comps[i]->batchExport){
				loadCompositionAtIndex(i);
				break;
			}
		}
		
		startRenderMode = false;
		currentlyRendering = true;
		saveFolder = currentCompositionDirectory + "rendered"+pathDelim;
		ofDirectory outputDirectory(saveFolder);
		if(!outputDirectory.exists()) outputDirectory.create(true);
		hiResPlayer->play();
		hiResPlayer->setSpeed(0);
		hiResPlayer->setVolume(0);
		
		renderer.setRGBTexture(*hiResPlayer);
		renderer.setTextureScale(1.0, 1.0);
		//		currentSimplify = 1;
		lastRenderFrame = currentRenderFrame-1;
		numFramesToRender = timeline.getOutFrame() - timeline.getInFrame();
		numFramesRendered = 0;
		currentLockCamera = cameraTrack.lockCameraToTrack = true;
		cameraTrack.setTimelineInOutToTrack();
		currentRenderFrame = timeline.getInFrame();
	}
	
	if(currentlyRendering){
		//hiResPlayer->setFrame(currentRenderFrame % hiResPlayer->getTotalNumFrames());
		timeline.setCurrentFrame(currentRenderFrame);
		videoTimelineElement.selectFrame(currentRenderFrame);		
		hiResPlayer->setFrame(videoTimelineElement.selectFrame(currentRenderFrame));
		hiResPlayer->update();
		
//		cout << "would have set hi res frame to " << currentRenderFrame % hiResPlayer->getTotalNumFrames() << endl;
//		cout << "instead set it to " << hiResPlayer->getCurrentFrame() << endl;
		
		////////
		//		char filename[512];
		//		sprintf(filename, "%s/TEST_FRAME_%05d_%05d_A.png", saveFolder.c_str(), currentRenderFrame, hiResPlayer->getCurrentFrame());
		//		savingImage.saveImage(filename);		
		//		savingImage.setFromPixels(hiResPlayer->getPixelsRef());
		//		savingImage.saveImage(filename);
		//		
		//		cout << "FRAME UPDATE" << endl;
		//		cout << "	setting frame to " << currentRenderFrame << " actual frame is " << hiResPlayer->getCurrentFrame() << endl;
		//		cout << "	set to percent " << 1.0*currentRenderFrame/hiResPlayer->getTotalNumFrames() << " actual percent " << hiResPlayer->getPosition() << endl;
		////////
		
		updateRenderer(*hiResPlayer);		
	}
	
	if(!currentlyRendering){
		lowResPlayer->update();	
		if(!temporalAlignmentMode && lowResPlayer->isFrameNew()){
			updateRenderer(*lowResPlayer);
		}
		//cout << "timeline is " << videoTimelineElement.getSelectedFrame() << " and player is " << lowResPlayer->getCurrentFrame() << endl;; 
		if(temporalAlignmentMode && (currentDepthFrame != depthSequence.getSelectedFrame())){
			updateRenderer(*lowResPlayer);
		}
		
		if(sampleCamera){
			cameraTrack.sample();
		}
				
		if(captureFramePair && temporalAlignmentMode){
			alignmentScrubber.registerCurrentAlignment();
			alignmentScrubber.save();
			captureFramePair = false;
		}
	}
	
	if(shouldClearCameraMoves){
		cameraTrack.getCameraTrack().reset();
		shouldClearCameraMoves = false;
	}	
	
	if(shouldSaveCameraPoint){
		//cameraRecorder.sample(lowResPlayer->getCurrentFrame());
		cameraTrack.getCameraTrack().sample(timeline.getCurrentFrame());
		shouldSaveCameraPoint = false;	
	}
	
	if(shouldResetCamera){
		cam.targetNode.setPosition(ofVec3f());
		cam.targetNode.setOrientation(ofQuaternion());
		cam.targetXRot = -180;
		cam.targetYRot = 0;
		cam.rotationZ = 0;
		shouldResetCamera = false;
	}
	
//	newCompButton->enabled  = viewComps;
//	saveCompButton->enabled = viewComps;
	
    renderer.edgeCull = timeline.getKeyframeValue("Edge Snip");
    renderer.farClip  = timeline.getKeyframeValue("Z Threshold");
	renderer.meshRotate.x = timeline.getKeyframeValue("X Rotate");
    renderer.meshRotate.y = timeline.getKeyframeValue("Y Rotate");
    renderer.meshRotate.z = timeline.getKeyframeValue("Z Rotate");
    
	if(currentXAdditiveShift != renderer.yshift ||
	   currentYAdditiveShift != renderer.yshift ||
	   currentXMultiplyShift != renderer.xmult ||
	   currentYMultiplyShift != renderer.ymult ||
	   currentXScale != renderer.xscale ||
	   currentYScale != renderer.yscale ||
	   currentRotationCompensation != renderer.rotationCompensation ||
	   currentSimplify != renderer.getSimplification() ||
//	   currentEdgeCull != renderer.edgeCull ||
//	   farClip != renderer.farClip ||
	   currentMirror != renderer.mirror ||
	   currentZFuzz != renderer.ZFuzz ||
	   fillHoles != holeFiller.enable ||
	   currentHoleKernelSize != holeFiller.getKernelSize() ||
       currentHoleFillIterations != holeFiller.getIterations())
	{		
		renderer.xshift = currentXAdditiveShift;
		renderer.yshift = currentYAdditiveShift;
		renderer.xmult = currentXMultiplyShift;
		renderer.ymult = currentYMultiplyShift;
		renderer.xscale = currentXScale;
		renderer.yscale = currentYScale;
		renderer.setSimplification(currentSimplify);
		renderer.ZFuzz = currentZFuzz;
		renderer.mirror = currentMirror;
//		renderer.edgeCull = currentEdgeCull;
//		renderer.farClip = farClip;
		
		holeFiller.enable = fillHoles;
		holeFiller.setKernelSize(currentHoleKernelSize);
		currentHoleKernelSize = holeFiller.getKernelSize();
		holeFiller.setIterations(currentHoleFillIterations);
		currentHoleFillIterations = holeFiller.getIterations();
        
		//renderer.update();
		updateRenderer(*lowResPlayer);
	}
	
    if(timeline.getUserChangedValue()){
    	updateRenderer(*lowResPlayer);
    }
    
	//update shaders
//	renderer.fadeToWhite = timeline.getKeyframeValue("White");


	if(!temporalAlignmentMode && !currentlyRendering && lowResPlayer->getSpeed() == 0.0){
		videoTimelineElement.selectFrame(timeline.getCurrentFrame());
	}	
}


//--------------------------------------------------------------
void testApp::updateRenderer(ofVideoPlayer& fromPlayer){
	
	if (!temporalAlignmentMode) {
		if(alignmentScrubber.getPairSequence().isSequenceTimebased()){
			long movieMillis = fromPlayer.getPosition() * fromPlayer.getDuration()*1000;
			currentDepthFrame = alignmentScrubber.getPairSequence().getDepthFrameForVideoFrame(movieMillis);
			depthSequence.selectTime(currentDepthFrame);
		}
		else {
			currentDepthFrame = alignmentScrubber.getPairSequence().getDepthFrameForVideoFrame(fromPlayer.getCurrentFrame());
			depthSequence.selectFrame(currentDepthFrame);
		}
		renderer.setDepthImage(depthPixelDecodeBuffer);
	}
	else{
		lowResPlayer->setFrame(videoTimelineElement.getSelectedFrame());
		renderer.setDepthImage(depthSequence.currentDepthRaw);
	}

    holeFilledPixels.setFromExternalPixels(depthSequence.currentDepthRaw, 640, 480, 1);
    holeFiller.close(holeFilledPixels);
	
    processDepthFrame();
    
    renderer.setDepthImage(holeFilledPixels.getPixels());	
	renderer.update();
	processGeometry();
	
	if(!drawPointcloud && !drawWireframe && !drawMesh){
		drawPointcloud = true;
	}	
	
	currentDepthFrame = depthSequence.getSelectedFrame();

}

//--------------------------------------------------------------
void testApp::draw(){	
	ofBackground(255*.2);
	if(allLoaded){
		
		if(!viewComps){
            
			//cout << timeline.getDrawRect().height << " tl height " << endl;
			if(!ofGetMousePressed(0)){
				timeline.setOffset(ofVec2f(0, ofGetHeight() - timeline.getDrawRect().height));
			}
            
			fboRectangle.height = (timeline.getDrawRect().y - fboRectangle.y - 20);
			fboRectangle.width = 16.0/9.0*fboRectangle.height;
			ofDrawBitmapString(currentCompositionDirectory + " -- " + lastSavedDate, ofPoint(fboRectangle.x, fboRectangle.y-15));
			
			if(presentMode){
				fboRectangle.x = 0;
				fboRectangle.y = 0;
				fboRectangle.height = ofGetHeight();
				fboRectangle.width = 16.0/9.0*fboRectangle.height;
			}
			else {
				fboRectangle.x = 250;
				fboRectangle.y = 100;
				fboRectangle.height = (timeline.getDrawRect().y - fboRectangle.y - 20);
				fboRectangle.width = 16.0/9.0*fboRectangle.height;
				ofDrawBitmapString(currentCompositionDirectory, ofPoint(fboRectangle.x, fboRectangle.y-15));
			}
            
            drawGeometry();
            
			if(temporalAlignmentMode){
                colorAlignAssistRect = ofRectangle(fboRectangle.x + fboRectangle.width, fboRectangle.y, fboRectangle.width/3, fboRectangle.height/3);
                float ratio = colorAlignAssistRect.width / lowResPlayer->getWidth();
                depthAlignAssistRect = ofRectangle(colorAlignAssistRect.x, colorAlignAssistRect.y+colorAlignAssistRect.height,
                                                   640 * ratio, 480 * ratio);
                lowResPlayer->draw(colorAlignAssistRect);
				depthSequence.currentDepthImage.draw(depthAlignAssistRect);
            }
			if(currentlyRendering){
                ofFbo& fbo =  curbuf == 0 ? fbo1 : fbo2;
				fbo.getTextureReference().readToPixels(savingImage.getPixelsRef());
				char filename[512];
				sprintf(filename, "%s/save_%05d.png", saveFolder.c_str(), currentRenderFrame);
				savingImage.saveImage(filename);
				
				//cout << "at save time its set to " << hiResPlayer->getCurrentFrame() << endl;

				///////frame debugging
				//		numFramesRendered++;
				//		cout << "	Rendered (" << numFramesRendered << "/" << numFramesToRender << ") +++ current render frame is " << currentRenderFrame << " quick time reports frame " << hiResPlayer->getCurrentFrame() << endl;
				//		sprintf(filename, "%s/TEST_FRAME_%05d_%05d_B.png", saveFolder.c_str(), currentRenderFrame, hiResPlayer->getCurrentFrame());
				//		savingImage.saveImage(filename);
				//		savingImage.setFromPixels(hiResPlayer->getPixelsRef());
				//		savingImage.saveImage(filename);
				//////
				
				//stop when finished
				currentRenderFrame++;
				if(currentRenderFrame > timeline.getOutFrame()){
					finishRender();
				}
			}
			
//			if(sampleCamera){
//				ofDrawBitmapString("RECORDING CAMERA", ofPoint(600, 10));
//			}
		}
		
		gui.setDraw(!currentlyRendering && !presentMode);
		if(!presentMode && !viewComps){
			timeline.draw();
		}
		
		if(!presentMode){
			gui.draw();
		}
		
		ofSetColor(255);
	}
	
	if(viewComps){
		ofPushStyle();
		for(int i = 0; i < comps.size(); i++){
			if(comps[i]->wasRenderedInBatch){
				ofSetColor(50,200,100, 200);
				ofRect(*comps[i]->toggle);
			}
			else if(comps[i]->batchExport){
				ofSetColor(255,255,100, 200);
				ofRect(*comps[i]->toggle);
			}
		}
		ofPopStyle();
	}
}


#pragma mark compositions
//--------------------------------------------------------------
bool testApp::loadNewProject(){
	allLoaded = false;
	ofSystemAlertDialog("Select Someone's Name");
	ofFileDialogResult r;
	r = ofSystemLoadDialog("Get Directory", true);
	if(!r.bSuccess){
		return false;
	}
		
	string currentMediaFolder = r.getPath();	
	
	ofDirectory compBin(currentMediaFolder + pathDelim + "compositions"+pathDelim);
	if(!compBin.exists()){
		compBin.create(true);
	}	
	compBin.listDir();
	
	int compNumber = compBin.numFiles();
	currentCompositionDirectory = currentMediaFolder + pathDelim+"compositions"+pathDelim+"comp" + ofToString(compNumber) + pathDelim;
	currentCompIndex = compNumber;
	
	if(!loadAssetsFromCompositionDirectory(currentMediaFolder)){
		return false;
	}
	
	saveComposition();
	refreshCompButtons();
	loadTimelineFromCurrentComp();
	
	allLoaded = true;
	return true;
}

bool testApp::loadAssetsFromCompositionDirectory(string currentMediaFolder) {
//	cout << "loading media folder " << currentMediaFolder << endl;
	
	if(!playerElementAdded){
		populateTimelineElements();
	}
	
	ofDirectory dataDirectory(currentMediaFolder);
	dataDirectory.listDir();
	
	string currentCompositionFile = currentCompositionDirectory+"compositionsettings.xml";
	projectsettings.loadFile(currentCompositionFile);
	
	int numFiles = dataDirectory.numFiles();
	string calibrationDirectory = "";
	string videoPath = "";
	string smallVideoPath = "";
	string depthImageDirectory = "";
	pairingsFile = "";
	
	for(int i = 0; i < numFiles; i++){
		string testFile = dataDirectory.getName(i);
		if(testFile.find("calibration") != string::npos){
			calibrationDirectory = dataDirectory.getPath(i);
		}
		
		if(testFile.find("depth") != string::npos || testFile.find("TAKE") != string::npos){
			depthImageDirectory = dataDirectory.getPath(i);
		}
		
		if(testFile.find("mov") != string::npos || testFile.find("MOV") != string::npos ){
			if(testFile.find("small") == string::npos){
				videoPath = dataDirectory.getPath(i);
			}
			else {
				smallVideoPath = dataDirectory.getPath(i);
			}
		}		
		
		if(testFile.find("pairings") != string::npos){
			pairingsFile = dataDirectory.getPath(i);
		}

	}
	
	if(smallVideoPath == ""){
		ofSystemAlertDialog("Error loading composition " + currentCompositionDirectory + " no Small Video File found.");
		return false;
	}

	if(calibrationDirectory == ""){
		ofSystemAlertDialog("Error loading composition " + currentCompositionDirectory + ". No calibration/ directory found.");
		return false;	
	}

	if(depthImageDirectory == ""){
		ofSystemAlertDialog("Error loading composition " + currentCompositionDirectory + ". No Depth directory found. Make sure the folder containing the depth images has 'depth' in the name");
		return false;	
	}
	
	
	if(pairingsFile == ""){
		pairingsFile = ofFilePath::removeExt(smallVideoPath) + "_pairings.xml";
	}
	cout << "********** frameExtracted " << calibrationDirectory << endl;
	if(!loadAlignmentMatrices(calibrationDirectory)){
		ofSystemAlertDialog("Load Failed -- Couldn't Load Calibration Directory.");
		return false;
	}
	
	if(!loadVideoFile(videoPath, smallVideoPath)){
		ofSystemAlertDialog("Load Failed -- Couldn't load video.");
		return false;
	}
	
	if(!loadDepthSequence(depthImageDirectory)){
		ofSystemAlertDialog("Load Failed -- Couldn't load depth iamges.");
		return false;
	}
	
	cam.cameraPositionFile = currentCompositionDirectory + "camera_position.xml";
	cam.loadCameraPosition();
	alignmentScrubber.setXMLFileName(pairingsFile);
	alignmentScrubber.load();
    if(!alignmentScrubber.ready()){
    	gui.setPage(4);
		shouldResetCamera = true;
        videoTimelineElement.selectFrame(videoTimelineElement.videoThumbs.size()/2);
        depthSequence.selectFrame(depthSequence.videoThumbs.size()/2);
        currentSimplify = 4;
    }
    currentDuration = lowResPlayer->getTotalNumFrames();
    
	ofDirectory compFolder(currentCompositionDirectory);
	if(!compFolder.exists()){
		compFolder.create(true);
	}
	return true;
}

//--------------------------------------------------------------
bool testApp::loadDepthSequence(string path){
	
	depthSequence.setup();
	
	depthPixelDecodeBuffer = depthSequence.currentDepthRaw;
	renderer.setDepthImage(depthPixelDecodeBuffer);
	
	return depthSequence.loadSequence(path);
}

//--------------------------------------------------------------
bool testApp::loadVideoFile(string hiResPath, string lowResPath){

	hasHiresVideo = (hiResPath != "");
	if(hasHiresVideo){
		if(hiResPlayer != NULL){
			delete hiResPlayer;
		}
		hiResPlayer = new ofVideoPlayer();

		if(!hiResPlayer->loadMovie(hiResPath)){
			ofSystemAlertDialog("Load Failed -- Couldn't load hi res video file.");
			return false;
		}
	}
	
	//must at least have a low res player
	//string lowResPath = ofFilePath::removeExt(path) + "_small.mov";
	if(lowResPlayer != NULL){
		delete lowResPlayer;
	}
	lowResPlayer = new ofVideoPlayer();
	if(!lowResPlayer->loadMovie(lowResPath)){
		ofSystemAlertDialog("Load Failed -- Couldn't load low res video file.");
		return false;		
	}
	
	if(hasHiresVideo){
		renderer.setTextureScale(1.0*lowResPlayer->getWidth()/hiResPlayer->getWidth(), 
								 1.0*lowResPlayer->getHeight()/hiResPlayer->getHeight());
	}
	else { //refer to the calibration stats

		float fullsizedWidth  = renderer.getRGBCalibration().getDistortedIntrinsics().getImageSize().width;
		float fullsizedHeight = renderer.getRGBCalibration().getDistortedIntrinsics().getImageSize().height; 
//		cout << "image size is " << fullsizedWidth << " " << fullsizedHeight << endl;
		renderer.setTextureScale(1.0*lowResPlayer->getWidth()/fullsizedWidth, 
								 1.0*lowResPlayer->getHeight()/fullsizedHeight);
	}
	
	renderer.setRGBTexture(*lowResPlayer);
	string videoThumbsPath = ofFilePath::removeExt(lowResPath);
	if(!ofDirectory(videoThumbsPath).exists()){
		ofDirectory(videoThumbsPath).create(true);
	}
	videoTimelineElement.setup();	
	if(!enableVideoInOut){
		timeline.setDurationInFrames(lowResPlayer->getTotalNumFrames());
	}
	videoTimelineElement.setVideoPlayer(*lowResPlayer, videoThumbsPath);
	lowResPlayer->play();
	lowResPlayer->setSpeed(0);
	
	return true;
}

//--------------------------------------------------------------
void testApp::loadTimelineFromCurrentComp(){

	alignmentScrubber.setup();
	alignmentScrubber.videoSequence = &videoTimelineElement;
	alignmentScrubber.depthSequence = &depthSequence;
	
    timeline.loadElementsFromFolder(currentCompositionDirectory);
    
    //fix up pairings file back into the main dir
    alignmentScrubber.setXMLFileName(pairingsFile);
    alignmentScrubber.load();
    
	string cameraSaveFile = currentCompositionDirectory + "camera.xml";
	cameraTrack.setXMLFileName(cameraSaveFile);
	cameraTrack.setup();	
    cameraTrack.load();
}

//--------------------------------------------------------------
bool testApp::loadAlignmentMatrices(string path){
	return renderer.setup(path);
}

//--------------------------------------------------------------
void testApp::loadCompositions(){
	ofSystemAlertDialog("Select the MediaBin");

	ofFileDialogResult r = ofSystemLoadDialog("Select Media Bin", true);
	if(r.bSuccess){
		mediaBinDirectory = r.getPath();
		refreshCompButtons();
	}
}

//--------------------------------------------------------------
void testApp::refreshCompButtons(){
	ofDirectory dir(mediaBinDirectory);
	dir.listDir();
	int mediaFolders = dir.numFiles();
	int currentCompButton = 0;
	int compx = 300;
	int compy = 150;
	for(int i = 0; i < mediaFolders; i++){
		
		string compositionsFolder = dir.getPath(i) + pathDelim +"compositions"+pathDelim;
		ofDirectory compositionsDirectory(compositionsFolder);
		if(!compositionsDirectory.exists()){
			compositionsDirectory.create(true);
		}
		
		compositionsDirectory.listDir();
		int numComps = compositionsDirectory.numFiles();
		//int compx = newCompButton->x+newCompButton->width+25;
		for(int c = 0; c < numComps; c++){
			Comp* comp;
			if(currentCompButton >= comps.size()){
				comp = new Comp();
				comp->load  = new ofxMSAInteractiveObjectWithDelegate();
				comp->load->setup();
				comp->load->setDelegate(this);
				
				comp->toggle = new ofxMSAInteractiveObjectWithDelegate();
				comp->toggle->setup();
				comp->toggle->setDelegate(this);				
				comps.push_back(comp);
			}
			else{
				comp = comps[currentCompButton];
			}
			comp->batchExport = false;
			comp->wasRenderedInBatch = false;

			comp->toggle->setPosAndSize(compx, compy,25,25);
			comp->load->setPosAndSize(compx+25, compy, 300, 25);
			
			compy+=25;
			if(compy > ofGetHeight()-100){
				compy  = 150;
				compx += 325;
			}

			comp->fullCompPath = compositionsDirectory.getPath(c);
			vector<string> compSplit = ofSplitString(comp->fullCompPath, pathDelim, true, true);
			string compLabel = compSplit[compSplit.size()-3] + ":" + compSplit[compSplit.size()-1];
			
			comp->load->setLabel(compLabel);
			if(currentCompositionDirectory == comp->fullCompPath){
				currentCompIndex = currentCompButton;
			}	
			currentCompButton++;
		}
	}
}

//--------------------------------------------------------------
void testApp::newComposition(){	
	loadNewProject();
}

//--------------------------------------------------------------
void testApp::saveComposition(){
	
	cam.saveCameraPosition();
	cameraTrack.save();
    
	projectsettings.setValue("drawPointcloud", drawPointcloud);
	projectsettings.setValue("drawWireframe", drawWireframe);
	projectsettings.setValue("drawMesh", drawMesh);
    projectsettings.setValue("drawDepthDistortion", drawDepthDistortion);
	projectsettings.setValue("drawGeometryDistortion", drawGeometryDistortion);
    
	projectsettings.setValue("cameraSpeed", cam.speed);
	projectsettings.setValue("cameraRollSpeed", cam.rollSpeed);
	projectsettings.setValue("xmult", currentXMultiplyShift);
	projectsettings.setValue("ymult", currentYMultiplyShift);
	projectsettings.setValue("xshift", currentXAdditiveShift);
	projectsettings.setValue("yshift", currentYAdditiveShift);
	projectsettings.setValue("xscale", currentXScale);
	projectsettings.setValue("yscale", currentYScale);
	
    projectsettings.setValue("fillholes", fillHoles);
    projectsettings.setValue("kernelSize", currentHoleKernelSize);
    projectsettings.setValue("holeIterations", currentHoleFillIterations);
    projectsettings.setValue("zfuzz", currentZFuzz);
    
	projectsettings.setValue("pointcloud", drawPointcloud);
	projectsettings.setValue("wireframe", drawWireframe);
	projectsettings.setValue("mesh", drawMesh);
	projectsettings.setValue("simplify",currentSimplify);
	projectsettings.setValue("mirror", currentMirror);
	projectsettings.setValue("duration", currentDuration);
	
	projectsettings.setValue("videoinout", enableVideoInOut);
	projectsettings.setValue("videoin", videoInPercent);
	projectsettings.setValue("videoout", videoOutPercent);
	
//	projectsettings.setValue("xrot", renderer.rotateMeshX);
	
	projectsettings.saveFile();
    
    lastSavedDate = "Last Saved on " + ofToString(ofGetMonth() ) + "/" + ofToString( ofGetDay()) + " at " + ofToString(ofGetHours()) + ":" + ofToString( ofGetMinutes() )  + ":" + ofToString( ofGetSeconds() );
}

void testApp::objectDidRollOver(ofxMSAInteractiveObject* object, int x, int y){
}

void testApp::objectDidRollOut(ofxMSAInteractiveObject* object, int x, int y){
}

void testApp::objectDidPress(ofxMSAInteractiveObject* object, int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::objectDidRelease(ofxMSAInteractiveObject* object, int x, int y, int button){
	if(object == newCompButton){
		newComposition();
	}
	else if(object == saveCompButton){
		saveComposition();		
	}
	else {
		for(int i = 0; i < comps.size(); i++){
			
			if(comps[i]->toggle == object){
				comps[i]->wasRenderedInBatch = false;
				comps[i]->batchExport = !comps[i]->batchExport;
				break;
			}

			if(comps[i]->load == object){
				loadCompositionAtIndex(i);
				break;
			}		
		}
	}
}
			
//--------------------------------------------------------------
bool testApp::loadCompositionAtIndex(int i){
	stopCameraRecord();
	
	currentCompositionDirectory = comps[i]->fullCompPath + pathDelim;
	currentCompIndex = i;

	allLoaded = loadAssetsFromCompositionDirectory( currentCompositionDirectory+".."+pathDelim+".."+pathDelim );
	if(!allLoaded){
		return false;
	}
	
	cam.speed = projectsettings.getValue("cameraSpeed", 20.);
	cam.rollSpeed = projectsettings.setValue("cameraRollSpeed", 1);
	
	drawPointcloud = projectsettings.getValue("drawPointcloud", true);
	drawWireframe = projectsettings.getValue("drawWireframe", true);
	drawMesh = projectsettings.getValue("drawMesh", true);
	drawDepthDistortion = projectsettings.getValue("drawDepthDistortion", true);
	drawGeometryDistortion= projectsettings.getValue("drawGeometryDistortion", true);
    
	currentXMultiplyShift = projectsettings.getValue("xmult", 0.);
	currentYMultiplyShift = projectsettings.getValue("ymult", 0.);
	currentXAdditiveShift = projectsettings.getValue("xshift", 0.);
	currentYAdditiveShift = projectsettings.getValue("yshift", 0.);
	currentXScale = projectsettings.getValue("xscale", 1.0);
	currentYScale = projectsettings.getValue("yscale", 1.0);
	
//	currentEdgeCull = projectsettings.getValue("edgeCull", 50);
//	farClip = projectsettings.getValue("farClip", 5000);
	drawPointcloud = projectsettings.getValue("pointcloud", false);
	drawWireframe = projectsettings.getValue("wireframe", false);
	drawMesh = projectsettings.getValue("mesh", false);
	currentSimplify = projectsettings.getValue("simplify", 1);
	currentMirror = projectsettings.getValue("mirror", false);
	currentDuration = projectsettings.getValue("duration", int(videoTimelineElement.videoThumbs.size()) );
	enableVideoInOut = projectsettings.getValue("videoinout", false);
	videoInPercent = projectsettings.getValue("videoin", 0.);
	videoOutPercent = projectsettings.getValue("videoout", 1.);
	//renderer.rotateMeshX = projectsettings.getValue("xrot", 0);
	
    fillHoles = projectsettings.getValue("fillholes", false);
    currentHoleKernelSize = projectsettings.getValue("kernelSize", 1);
    currentHoleFillIterations = projectsettings.getValue("holeIterations", 1);
	currentZFuzz = projectsettings.getValue("zfuzz", 0.);

	//error condition for corrupted comps
	//if(currentDuration <= 0){
		currentDuration = lowResPlayer->getTotalNumFrames();
	//}
//	if(farClip <= 0){
//		farClip = 5000;
//	}
	shouldResetDuration = true;
	
	//set keyframer files based on comp
	loadTimelineFromCurrentComp();

	//LOAD CAMERA SAVE AND POS
	cam.cameraPositionFile = currentCompositionDirectory + "camera_position.xml";
	cam.loadCameraPosition();
    
//	timeline.setCurrentPage("Main");

    //turn off view comps
	viewComps = false;
}	

//--------------------------------------------------------------
void testApp::objectDidMouseMove(ofxMSAInteractiveObject* object, int x, int y){

}

//--------------------------------------------------------------
void testApp::finishRender(){
	currentlyRendering = false;
	comps[currentCompIndex]->batchExport = false;
	comps[currentCompIndex]->wasRenderedInBatch = true;
	for(int i = currentCompIndex+1; i < comps.size(); i++){
		if(comps[i]->batchExport){
			loadCompositionAtIndex(i);
			startRenderMode = true;
			return;
		}
	}
	
	//no more left, we are done
	//stopCameraPlayback();
	renderer.setRGBTexture(*lowResPlayer);
	renderer.setTextureScale(1.0*lowResPlayer->getWidth()/hiResPlayer->getWidth(), 
							 1.0*lowResPlayer->getHeight()/hiResPlayer->getHeight());
}


void testApp::startCameraRecord(){
	if(!sampleCamera){
		cameraTrack.lockCameraToTrack = false;
		//cameraRecorder.reset();
		cameraTrack.getCameraTrack().reset();
		sampleCamera = true;
		lowResPlayer->setSpeed(1.0);
	}	
}

//--------------------------------------------------------------
void testApp::stopCameraRecord(){
	sampleCamera = false;
}

//--------------------------------------------------------------
void testApp::toggleCameraRecord(){
	if(sampleCamera){
		stopCameraRecord();
	}
	else{
		startCameraRecord();
	}
}

//--------------------------------------------------------------
void testApp::toggleCameraPlayback(){
	cameraTrack.lockCameraToTrack = !cameraTrack.lockCameraToTrack;
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	timeline.setWidth(w);
	timeline.setOffset(ofVec2f(0, ofGetHeight() - timeline.getDrawRect().height));
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}