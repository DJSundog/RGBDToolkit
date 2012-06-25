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
//	currentSimplify = 2;
	
	videoInPercent = 0.0;
	videoOutPercent = 1.0;
	enableVideoInOut = false;
	
//	currentZFuzz = 0;
    
    /*----------------------------------*
     Set Default Values for GUI variables here
     *----------------------------------*/
    selfOcclude = false;
    
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
	shouldExportSettings = false;
    rendererDirty = true;
    

    drawFaceTracker = false;

    /*----------------------------------*
     Feature Detection Variables 
     
     featureDraw :   toggle on and off to draw 
                     the detected points on the img
     featureMax :    number of points you would like
                     returned
     featureQuality: slide between .02 and .001, determines
                     how "interesting" potential points 
                     should be.
     featureMinDist: minimum distance between points
     
     *----------------------------------*/
    featureDraw = false;
    fillFDMeshTri = true;
    drawfDContour = false;
    
    featureMax = 1500;
    featureQuality = 0.001;
    featureMinDist = 4;
    fDimgSat = 30;
    
    fDcrop = 20;
    
    dTriangles.setMaxPoints(featureMax);
    
    cFminRadius = 70;
    cFmaxRadius = 500;
    cFthresh = 36;
    cFmaxDist = 80;
    
    //countourFinder setup, copied from ContoursTrackingExample
    //from ofCv by kylemcdonald
    
    contourFinder.setMinAreaRadius(cFminRadius);
	contourFinder.setMaxAreaRadius(cFmaxRadius);
	contourFinder.setThreshold(cFthresh);
	// wait for half a frame before forgetting something
	contourFinder.getTracker().setPersistence(15);
	// an object can move up to 32 pixels per frame
	contourFinder.getTracker().setMaximumDistance(cFmaxDist);
    
    /*----------------------------------*/
    
    
    //Allocate and initialize FBOs 
    //TODO set through interface
    int fboWidth  = 1920;
    int fboHeight = 1080;
	
	savingImage.setUseTexture(false);
	savingImage.allocate(fboWidth,fboHeight, OF_IMAGE_COLOR);
	
	fboRectangle = ofRectangle(250, 100, fboWidth, fboHeight);
    swapFbo.allocate(fboWidth, fboHeight, GL_RGBA, 4);
	curbuf = 0;
    /*
    fbo1.allocate(fboWidth, fboHeight, GL_RGBA32F_ARB);
	fbo2.allocate(fboWidth, fboHeight, GL_RGBA32F_ARB);
    dofBuffer.allocate(fboWidth, fboHeight, GL_RGB32F_ARB, 4);
    dofBlurBuffer.allocate(fboWidth, fboHeight, GL_RGB32F_ARB);
    */
    fbo1.allocate(fboWidth, fboHeight, GL_RGBA);
	fbo2.allocate(fboWidth, fboHeight, GL_RGBA);
    dofBuffer.allocate(fboWidth, fboHeight, GL_RGB, 4);
    dofBlurBuffer.allocate(fboWidth, fboHeight, GL_RGBA);

    
    fbo1.begin();
    ofClear(0,0,0,0);
    fbo1.end();
    fbo2.begin();
    ofClear(0,0,0,0);
    fbo2.end();
    dofBuffer.begin();
    ofClear(0,0,0,0);
    dofBuffer.end();
    dofBlurBuffer.begin();
    ofClear(0,0,0,0);
    dofBlurBuffer.end();
    
    
    loadShaders();
    
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

    /*----------------------------------*
     Gui Features
     
     Add new pages of sliders and toggles here
     
     Remeber to initialize variables above, and
     set a hot key in the keyPressed() function 
     for the user.
     
     TODO:: some sort of visual guide to
            what the hot keys do on the
            main page
    *----------------------------------*/
	gui.addToggle("Pause Render", pauseRender);
	gui.addToggle("Draw Pointcloud", drawPointcloud);
	gui.addToggle("Draw Wireframe", drawWireframe);
	gui.addToggle("Draw Mesh", drawMesh);
	gui.addToggle("Depth Distortion", drawDepthDistortion);
	gui.addToggle("Geometry Distortion", drawGeometryDistortion);
	gui.addToggle("Self Occlude", selfOcclude);
    gui.addToggle("Draw DOF", drawDOF);
    gui.addToggle("Face Tracking", drawFaceTracker);
    
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
	gui.addSlider("X Multiply Shift", currentXMultiplyShift, -.15, .15);
	gui.addSlider("Y Multiply Shift", currentYMultiplyShift, -.15, .15);
	gui.addToggle("Fill Holes", fillHoles);	
	gui.addSlider("Hole Kernel Size", currentHoleKernelSize, 1, 20);
	gui.addSlider("Hole Fill Iterations", currentHoleFillIterations, 1, 20);
	gui.addToggle("TemporalAlignmentMode", temporalAlignmentMode);
	gui.addToggle("Capture Frame Pair", captureFramePair);

	gui.addPage("Batching");
	gui.addToggle("View Comps", viewComps);
	gui.addToggle("Render Batch", startRenderMode);
	gui.addToggle("Export Settings", shouldExportSettings);
    
    gui.addPage("Feature Detection");
    gui.addToggle("Draw Abstrated Mesh", featureDraw);
    gui.addToggle("Hide Triangulization", fillFDMeshTri);
    gui.addToggle("Draw Contour Finder", drawfDContour);
    gui.addSlider("maxFeatures", featureMax, 1, 1000);
    gui.addSlider("qualityLevel", featureQuality, 0.02, 0.001);
    gui.addSlider("Minimum Point Distance", featureMinDist, 1, 16);
    gui.addSlider("Color Saturation", fDimgSat, 0, 100);
    gui.addSlider("Crop", fDcrop,0,100);
    gui.addSlider("Contour Finder min Object Radius", cFminRadius, 1, cFmaxRadius);
    gui.addSlider("Contour Finder max Object Radius", cFmaxRadius, cFminRadius, 1000);
    gui.addSlider("Contour Finder threshold", cFthresh, 15, 400);
    gui.addSlider("Contour Finder max Distance", cFmaxDist, 0,500);
    
	gui.loadFromXML();
	gui.toggleDraw();
	
	currentLockCamera = false;
	cameraTrack.lockCameraToTrack = false;
    
    accumulatedPerlinOffset = 0;
    
    faceTracker.setup();
    faceTrackerMesh = ofMesh();
    
    faceFilePresent = faceTrackerData.loadFile(currentCompositionDirectory + "trackingdata.xml");
}

void testApp::loadShaders(){
    dofRange.load("shaders/dofrange"); 
    dofBlur.load("shaders/dofblur");
    dofBlur.begin();
    dofBlur.setUniform1i("tex", 0);
    dofBlur.setUniform1i("range", 1);
    dofBlur.end();
    
    renderer.reloadShader();
}

//Labbers: YOU CAN ADD TIMELINE ELEMENTS HERE
void testApp::populateTimelineElements(){
	
	timeline.setPageName("Camera");
	timeline.addElement("Camera", &cameraTrack);
	timeline.addElement("Video", &videoTimelineElement);

    //rendering
    timeline.addPage("Global Rendering", true);
    timeline.addKeyframes("Motion Trail Decay", currentCompositionDirectory + "motionTrailDecay.xml", ofRange(.05 ,1.0), 1.0 );
    timeline.addKeyframes("Simplify", currentCompositionDirectory + "simplify.xml", ofRange(1, 8), 2);
    timeline.addKeyframes("Edge Snip", currentCompositionDirectory + "edgeSnip.xml", ofRange(1.0, 6000), 6000 );
    timeline.addKeyframes("Z Threshold", currentCompositionDirectory + "zThreshold.xml", ofRange(1.0, sqrtf(6000)), sqrtf(6000) );

    timeline.addPage("Mesh", true);
    timeline.addKeyframes("Mesh Alpha", currentCompositionDirectory + "meshAlpha.xml", ofRange(0,1.0) );
	timeline.addKeyframes("X Rotate", currentCompositionDirectory + "meshXRot.xml", ofRange(-360,360), 0.);
    timeline.addKeyframes("Y Rotate", currentCompositionDirectory + "meshYRot.xml", ofRange(-360,360), 0.);
    timeline.addKeyframes("Z Rotate", currentCompositionDirectory + "meshZRot.xml", ofRange(-360,360), 0.);
    
    timeline.addPage("Wireframe", true);
    timeline.addKeyframes("Wireframe Alpha", currentCompositionDirectory + "wireframeAlpha.xml", ofRange(0,1.0), 1.0 );
    timeline.addKeyframes("Wireframe Thickness", currentCompositionDirectory + "wireframeThickness.xml", ofRange(1.0,sqrtf(20.0)) );
     
    timeline.addPage("Point", true);
    timeline.addKeyframes("Point Alpha", currentCompositionDirectory + "pointAlpha.xml", ofRange(0,1.0) );
    timeline.addKeyframes("Point Size", currentCompositionDirectory + "pointSize.xml", ofRange(1.0, sqrtf(20.0) ) );	
    
    timeline.addPage("Depth of Field", true);
    timeline.addKeyframes("DOF Distance", currentCompositionDirectory + "DOFDistance.xml", ofRange(0,sqrtf(1000.0)), 10 );
    timeline.addKeyframes("DOF Range", currentCompositionDirectory + "DOFRange.xml", ofRange(10,sqrtf(500.0)) );
    timeline.addKeyframes("DOF Blur", currentCompositionDirectory + "DOFBlur.xml", ofRange(0,5.0) );

    timeline.addPage("Depth Distortion", true);
    timeline.addKeyframes("Noise", currentCompositionDirectory + "DepthNoise.xml", ofRange(0, 5000) );
    timeline.addKeyframes("Sine Amplitude", currentCompositionDirectory + "SineAmp.xml", ofRange(0, sqrtf(100)) );
    timeline.addKeyframes("Sine Frequency", currentCompositionDirectory + "SineFrequency.xml", ofRange(.00, 1) );
    timeline.addKeyframes("Sine Speed", currentCompositionDirectory + "SineSpeed.xml", ofRange(-sqrtf(1.5), sqrtf(1.5)), 0 );
    timeline.addKeyframes("Depth Blur", currentCompositionDirectory + "DepthBlur.xml", ofRange(0, 10), 0 );
    
    timeline.addPage("Geometry  Distortion", true);
    timeline.addKeyframes("Perlin Amp", "PerlinAmp.xml", ofRange(0, 200.0) );
    timeline.addKeyframes("Perlin Density", "PerlinDensity.xml", ofRange(0, 200.0) );
    timeline.addKeyframes("Perlin Speed", "PerlinSpeed.xml", ofRange(0, sqrtf(15.)) );    
    
//    timeline.addPage("Lumen Noise", true);
    
	timeline.addPage("Alignment", true);
	timeline.addElement("Video", &videoTimelineElement);
	timeline.addElement("Depth Sequence", &depthSequence);
	timeline.addElement("Alignment", &alignmentScrubber);
    
	timeline.setCurrentPage("Rendering");
	
	playerElementAdded = true;
    
}

#pragma mark customization

//The funciton below manipulates the raw kinect depth images,
//for example, if you increase noise in the timeline
//--------------------------------------------------------------

/*------------------------------------------------------------*
 ProcessDepthFrame
 
 CUSTOMIZE :: Manipulate the raw Kinect Data here.
 
 depthPixelDecodeBuffer contains the raw depth image. 
 currently only being used to add noise to the connect data
 
 *------------------------------------------------------------*/

void testApp::processDepthFrame(){
	
    if(!drawDepthDistortion) return;
    
    if(!depthSequence.currentDepthRaw.isAllocated()){
        return;
    }
    
    float noise = timeline.getKeyframeValue("Noise");
    float sineSpeed = timeline.getKeyframeValue("Sine Speed");
    float sineAmp = timeline.getKeyframeValue("Sine Amplitude");
    float sineFrequency = timeline.getKeyframeValue("Sine Frequency");
    int blur = timeline.getKeyframeValue("Depth Blur");
    
    sineFrequency *= sineFrequency;
    sineSpeed = sineSpeed;
    sineAmp *= sineAmp;

	for(int y = 0; y <	480; y++){
		for(int x = 0; x < 640; x++){
			int index = y*640+x;		
            if(noise > 0){
                //holeFilledPixels.getPixels()[index] += ofRandom(noise);   
                depthSequence.currentDepthRaw.getPixels()[index] += ofRandom(noise);  
            }
            
            if(sineAmp > 0 && depthSequence.currentDepthRaw.getPixels()[index] > 0){
                depthSequence.currentDepthRaw.getPixels()[index] += sin( y * sineFrequency + timeline.getCurrentFrame() * sineSpeed ) * sineAmp;
            }			
		}
	}
    
    //Mat m_element_m = getStructuringElement(MORPH_RECT, cv::Size(kernelSize, kernelSize));
    ofxCv::blur(depthSequence.currentDepthRaw, blur);
    
}

/*------------------------------------------------------------*
 Process the Mesh
 
 CUSTOMIZE :: Manipulate the mesh indicies/texture coords here
 
 renderer.getMesh() contains mesh with indeces and texture coordinates 

 A | okay, so this thing doesn't actually contain texture coordinates,
   | unless you flip the calculateTextureCoordinates boolean which
   | DEVASTATES your framerate. However, the vector containing the
   | verticies are indexed by the depth image cordinate 
   | so [y*w + x ] and contain a 
   | vec3f( xProjected, yProjected, zKinectCoordinate)
 
 Also keep in mind that not all indicies in the vectors in 
 renderer.getMesh() [which is a 640x480 1 dimensional vector]
 are filled with valid data (most are 0,0,0), since only the
 verticies that correspond to the kinect data grid are filled.
*------------------------------------------------------------*/

void testApp::processGeometry(){

    if(!drawGeometryDistortion) return;
    
    float contract = 0; //timeline.getKeyframeValue("Contract");
    float explode = 0; //timeline.getKeyframeValue("Explode");
    
    float perlinAmp = timeline.getKeyframeValue("Perlin Amp");
    float perlinDensity = timeline.getKeyframeValue("Perlin Density");
    float perlinSpeed = timeline.getKeyframeValue("Perlin Speed");
    perlinSpeed *= perlinSpeed;

    accumulatedPerlinOffset += perlinSpeed;
    
    ofVec3f center(0,0,0);
    for(int i = 0; i < renderer.getMesh().getVertices().size(); i++){
        center += renderer.getMesh().getVertex(i);
    }
    center /= renderer.getMesh().getVertices().size();
    
    for(int i = 0; i < renderer.getMesh().getVertices().size(); i++){
        ofVec3f& vert = renderer.getMesh().getVertices()[i];
        if(perlinAmp > 0){
            renderer.getMesh().getVertices()[i] += ofVec3f(ofSignedNoise(vert.x/perlinDensity, vert.y/perlinDensity, vert.z/perlinDensity, accumulatedPerlinOffset)*perlinAmp,
                                                           ofSignedNoise(vert.z/perlinDensity, vert.x/perlinDensity, vert.y/perlinDensity, accumulatedPerlinOffset)*perlinAmp,
                                                           ofSignedNoise(vert.y/perlinDensity, vert.z/perlinDensity, vert.x/perlinDensity, accumulatedPerlinOffset)*perlinAmp );
        }
        
        if(explode != 0){
            vert += (vert - center).normalize() * explode;
        }
    }
    
//	for(int i = 0; i < renderer.getMesh().getVertices().size(); i++){
//		renderer.getMesh().getVertices()[i].z += sin(i/30.0 + timeline.getCurrentFrame())*25;
//	}
	
}

/*------------------------------------------------------------*
 FEATURE DETECTION
 
 Runs ofxCv Feature Detection on the current video frame. 
 Returns a Vector of interesting feature points on the image.
 
 notes::
 
 lowResPlayer->getTextureReference()
    -returns pointer to texture
     containing current frame.
*------------------------------------------------------------*/

vector<ofPoint> testApp::featureDetect(){

    std::vector<cv::Point2f> points;
    points.clear();
    
    //feature detect works on B/W IMAGE ONLY.
    goodFeaturesToTrack(ofxCv::toCv(fDbwImg), points, featureMax, (double)featureQuality, (double)featureMinDist);
  //  return ofxCv::toOf(harrisPoints).getVertices();
    return ofxCv::toOf(points).getVertices();
}

/*------------------------------------------------------------*
  Get 3D Texture Coordinates
 
  input: vector of ofPoints chosen from the currentFrame, 
         falloff distance for point matching (e.g., this point is a match
         if it is within this dist) TODO: set to some function of the 
         simplify variable?
 
  output: vector of ofPoints with z data filled in from the depth info
 
  Finds closest texture coordinate to each point in the input mesh.
  Adds the z data from the corresponding vertex into the input array
 
  NOTE
  before calling, make sure that renderer.caclulateTextureCoordinates 
  is set to TRUE in the very beginning of the update loop.
  Do this with a boolean flag, since it slows down the framerate a ton.
 
  (might be something you want to precompute, since normally texture
   mapping is done on the shader and is therefore very fast. This
   is very slow)
 *------------------------------------------------------------*/
vector<ofPoint> testApp::get3DtextureCoordinates(vector<ofPoint> input, float acceptableDist){
    
    vector<ofVec3f> verts =  renderer.getMesh().getVertices();
    vector<ofPoint> texs;
    
    texs.clear();
    
    for(ofVec3f v : verts){
        v.normalize();
        v.x = v.x +1; 
        v.y = v.y + 1;
        
        v.x = v.x/2;
        v.y = v.y/2;
        
        v.y = 1 - v.y;
        v.x = 1 - v.x;
        
        
    }
    
 /*   ofVec2f* texCoords =  renderer.getMesh().getTexCoordsPointer();
    ofVec3f* verts =  renderer.getMesh().getVerticesPointer();
    
    for( ofPoint p : input){
        float minDist = INTER_MAX;
        int closestPointIndex;
        for( int i = 0; i < renderer.getMesh().getTexCoords().size(); i++){
            float dist = ofDist( (texCoords + i)->x, (texCoords + i)->y, p.x, p.y);
            if(dist < minDist){
                minDist = dist;
                closestPointIndex = i; 
            }
            if(minDist < acceptableDist)
                break;
        }
        p.z = (verts + closestPointIndex)->z;
    }

    return input;*/
    
}
/*------------------------------------------------------------*
 Update Feature Detection Mesh Images
 
 fills the two ofImages used for generating the triangulized
 feature detection mesh with the current video frame.
 
 Feature Detection runs on the fDbwImg, and the colorImg is used
 to determine the color of each triangle. Apply any image 
 manipulations you want to apply to the ENTIRE img here.
 
 (Its faster to tweak the color only on the reference pixel
 in the drawFDMesh function)
 *------------------------------------------------------------*/

 
void testApp::updateFDMeshImages(){

    //nonsense to convert current video frame to ofImage
    ofPixels vidPix;
    ofPixels vidPixLG;
    
    if(hasHiresVideo){
        vidPixLG = hiResPlayer->getPixelsRef();
    }
    //else
        vidPix = lowResPlayer->getPixelsRef();
    
    fDcolorImg.clear();
    fDcolorImg = ofImage(vidPix);
    
    fDcolorImg.crop(fDcrop/2, 0, fDcolorImg.width-fDcrop/2, fDcolorImg.height);  
    
    //apply any image manipulation you want to apply to
    //the entire image to improve feature detection here
    //Only the b/w image gets passed to feature detection,
    //so don't touch the color unless the transformation
    //improves it anyway, e.g. a s curves layer
    
    fDbwImg = ofImage(fDcolorImg.getPixelsRef());
    fDbwImg.setImageType(OF_IMAGE_GRAYSCALE);
    
}

/*------------------------------------------------------------*
 
    Update the Feature Detection Mesh itself.
 
 
  *------------------------------------------------------------*/
void testApp::updateFDMesh(){
    
    featurePoints.clear();
    updateFDMeshImages();
    
    if(drawfDContour){
        //reset contourFinder variables from the GUI
        contourFinder.setMinAreaRadius(cFminRadius);
        contourFinder.setMaxAreaRadius(cFmaxRadius);
        contourFinder.setThreshold(cFthresh);
        contourFinder.getTracker().setPersistence(15);
        contourFinder.getTracker().setMaximumDistance(cFmaxDist);
    
        contourFinder.findContours(toCv(fDcolorImg));
        triangle.clear();
        
      /*  for (int i=0; i<contourFinder.nBlobs; i++)
        {
            triangle.triangulate(contourFinder.blobs[i], max( 3.0f, (float)contourFinder.blobs[i].pts.size()/5));
        }*/

    }
    
    //pull new features from the current frames
    featurePoints = featureDetect();
    
    //get z coordinate from the depth image
    //   get3DtextureCoordinates(featurePoints, 2.0);
    
    //triangulate the found points using ofxDelauay
    dTriangles.reset();
    dTriangles.setMaxPoints(featureMax);
    
    for(ofPoint p: featurePoints){
        dTriangles.addPoint(p);}
    
    dTriangles.triangulate();
}

/*------------------------------------------------------------*
 Draw Feature Detection Mesh
 
 input: boolean flag, if true, draws 3D mesh, false draws 2D mesh
 
 draws the mesh triangulated from the feature detection
*------------------------------------------------------------*/

void testApp::drawFDMesh(bool draw3D){
    ofPushStyle();
    
    if(drawfDContour){
        contourFinder.draw();
    }
    else{
        if(fillFDMeshTri){
            for( int i=0; i<dTriangles.triangles.size(); i++ )
            {
                int colorIndex;
                float centerX, centerY;
                
                ofxDelaunayTriangle& tri = dTriangles.triangles[ i ];
                centerX = (tri.points[0].x + tri.points[1].x + tri.points[2].x)/3;
                centerY = (tri.points[0].y + tri.points[1].y + tri.points[2].y)/3;
                
                // t.points[ 0 ].y * img.width + t.points[ 0 ].x]
                ofFill();
                ofColor c = fDcolorImg.getColor(centerX, centerY);
                
                //any color manipulation here
                c.setSaturation(c.getSaturation() + fDimgSat);
                ofSetColor(c);
                
                if(!draw3D){
                    ofTriangle
                    (
                     tri.points[ 0 ].x,
                     tri.points[ 0 ].y,
                     tri.points[ 1 ].x,
                     tri.points[ 1 ].y,
                     tri.points[ 2 ].x,
                     tri.points[ 2 ].y
                     );
                }
                else {
                    ofTriangle
                    (
                     tri.points[ 0 ].x,
                     tri.points[ 0 ].y,
                     tri.points[ 0 ].z,
                     tri.points[ 1 ].x,
                     tri.points[ 1 ].y,
                     tri.points[ 1 ].z,
                     tri.points[ 2 ].x,
                     tri.points[ 2 ].y,
                     tri.points[ 2 ].z
                     );
                    
                }
            }
        }
        else{
            ofSetColor(255,0,0);
            ofNoFill();
            dTriangles.draw();
            
            //uncomment for points
            /*  ofSetColor(0, 255, 255);
             for(ofPoint p : featurePoints)
             ofEllipse(p.x, p.y, 2, 2);*/
        }
    }
    ofPopStyle();
}

void testApp::drawGeometry(){
    
	//***************************************************
	//CUSTOMIZATION: YOU CAN DRAW WHATEVER YOU WANT HERE TOO OR USE SHADERS
	//*
	//* draw whatever you want!
	//*
	//***************************************************


    float pointAlpha = timeline.getKeyframeValue("Point Alpha");
    float wireAlpha = timeline.getKeyframeValue("Wireframe Alpha");
    float meshAlpha = timeline.getKeyframeValue("Mesh Alpha");
	
    if(!alignmentScrubber.ready()){
        pointAlpha = 0;
        wireAlpha = 1.0;
        meshAlpha = 0.0;    
    }

    //helps eliminate zfight by translating the mesh occluder slightly back from the camera
    ofVec3f camTranslateVec = cam.getLookAtDir();    
    ofRectangle renderFboRect = ofRectangle(0, 0, fbo1.getWidth(), fbo1.getHeight());
    
    if(pauseRender){
        ofPushStyle();
        ofSetColor(255,0,0);
        
        ofRect(fboRectangle);
        fbo1.getTextureReference().draw(fboRectangle);
        ofPopStyle();
	    return;
    }
    
//    if(!rendererDirty){
//        cout << "not dirty before" << endl;
//    }
        
    rendererDirty |= (renderedCameraPos.getPosition() != cam.getPosition() || 
                      renderedCameraPos.getOrientationQuat() != cam.getOrientationQuat() );

    if(rendererDirty){
//        cout << "dirty from camera" << endl;
        
        renderedCameraPos.setPosition(cam.getPosition());
        renderedCameraPos.setOrientation(cam.getOrientationQuat());
   
        ofBlendMode blendMode = OF_BLENDMODE_SCREEN;
        
        //new way of doing trails that kills alpha backgrounds.
        fbo1.begin();
        ofPushStyle();
        ofEnableAlphaBlending();
        float decay = timeline.getKeyframeValue("Motion Trail Decay");
        decay *= decay; //exponential
        ofSetColor(0, 0, 0, decay*255);
        ofRect(renderFboRect);
        
        ofPopStyle();
        fbo1.end();

        if(drawMesh && meshAlpha > 0){
            
            swapFbo.begin();
            
            ofClear(0,0,0,0);
            glEnable(GL_DEPTH_TEST);
            cam.begin(renderFboRect);
            ofPushStyle();
            renderer.drawMesh();
            ofPopStyle();
            glDisable(GL_DEPTH_TEST);
            
            cam.end();
            
            swapFbo.end();
            
            //composite
            fbo1.begin();
            
            ofPushStyle();
            ofEnableAlphaBlending();
            //ofEnableBlendMode(OF_BLENDMODE_ADD);
            ofSetColor(255, 255, 255, meshAlpha*255);
            swapFbo.getTextureReference().draw(renderFboRect);
            
            ofPopStyle();
            
            fbo1.end();       
            
        }
        
        if(drawWireframe && wireAlpha > 0){
                
            swapFbo.begin();
            ofClear(0,0,0,0);
            
            cam.begin(renderFboRect);
            ofPushStyle();
            
            ofEnableAlphaBlending();
            glEnable(GL_DEPTH_TEST);
            
            if(selfOcclude){
                //occlude points behind the mesh
                ofPushMatrix();
                ofSetColor(0, 0, 0, 0);
                ofTranslate(camTranslateVec); //Moves whole 
                renderer.drawMesh(false);
                ofPopMatrix();
            }
            
            ofEnableAlphaBlending();
            //ofEnableBlendMode(blendMode);
            glEnable(GL_DEPTH_TEST);
            
            ofSetColor(255*wireAlpha);        
            float thickness = timeline.getKeyframeValue("Wireframe Thickness");
            thickness *= thickness;
            ofSetLineWidth(thickness);
            renderer.drawWireFrame();
            ofPopStyle();
            
            glDisable(GL_DEPTH_TEST);
            cam.end();
            
            swapFbo.end();
            
            //composite
            fbo1.begin();
            
            ofPushStyle();
            //ofEnableAlphaBlending();
            ofEnableBlendMode(blendMode);
            ofSetColor(255);
            swapFbo.getTextureReference().draw(renderFboRect);
            ofPopStyle();
            
            fbo1.end();
            
        }
        
        if(drawPointcloud && pointAlpha > 0){
            
            swapFbo.begin();
            ofClear(0,0,0,0);
            
            cam.begin(renderFboRect);
            
            ofPushStyle();
            //occlude points behind the mesh
            
            ofEnableAlphaBlending();
            glEnable(GL_DEPTH_TEST);
            if(selfOcclude){
                ofPushMatrix();
                ofSetColor(0, 0, 0, 0);
                ofTranslate(camTranslateVec);
                renderer.drawMesh(false);
                ofPopMatrix();
            }
            ofEnableAlphaBlending();
            ofSetColor(255*pointAlpha);
            glEnable(GL_POINT_SMOOTH); // makes circular points
            float pointSize = timeline.getKeyframeValue("Point Size");
            glPointSize(pointSize*pointSize);
            renderer.drawPointCloud();
            ofPopStyle();
            
            glDisable(GL_DEPTH_TEST);
            cam.end();
            
            swapFbo.end();
            
            //composite
            fbo1.begin();
            
            ofPushStyle();
            //ofEnableAlphaBlending();
            ofEnableBlendMode(blendMode);
            ofSetColor(255);
            swapFbo.getTextureReference().draw(renderFboRect);
            ofPopStyle();
            
            fbo1.end();
        }

        if(drawDOF){
            //render DOF
            float dofFocalDistance = timeline.getKeyframeValue("DOF Distance");
            dofFocalDistance*=dofFocalDistance;
            float dofFocalRange = timeline.getKeyframeValue("DOF Range");
            dofFocalRange*=dofFocalRange;
            float dofBlurAmount = timeline.getKeyframeValue("DOF Blur");
//            float fogNear = timeline.getKeyframeValue("Fog Near");
//            float fogRange = timeline.getKeyframeValue("Fog Range");
            
            //DRAW THE DOF B&W BUFFER
            dofBuffer.begin();
            ofClear(255,255,255,255);
            
            cam.begin(renderFboRect);
            glEnable(GL_DEPTH_TEST);
            
            dofRange.begin();
            dofRange.setUniform1f("focalDistance", dofFocalDistance);
            dofRange.setUniform1f("focalRange", dofFocalRange);
//            dofRange.setUniform1f("fogNear", fogNear);
//            dofRange.setUniform1f("fogRange", fogRange);
            
            ofDisableAlphaBlending();
            renderer.drawMesh(false);
            //renderer.drawWireFrame(false);
            dofRange.end();
            
            glDisable(GL_DEPTH_TEST);
            cam.end();
            
            dofBuffer.end();
            
            //composite
            fbo2.begin();
            ofClear(0.0,0.0,0.0,0.0);
            
            ofPushStyle();
            ofEnableBlendMode(blendMode);
            
            ofSetColor(255);
            dofBlur.begin();
            
            //mulit-text
            //our shader uses two textures, the top layer and the alpha
            //we can load two textures into a shader using the multi texture coordinate extensions
            glActiveTexture(GL_TEXTURE0_ARB);
            fbo1.getTextureReference().bind();
            
            glActiveTexture(GL_TEXTURE1_ARB);
            dofBuffer.getTextureReference().bind();
            
            dofBlur.setUniform2f("sampleOffset", dofBlurAmount, 0);
            //draw a quad the size of the frame
            glBegin(GL_QUADS);
            
            //move the mask around with the mouse by modifying the texture coordinates
            glMultiTexCoord2d(GL_TEXTURE0_ARB, 0, 0);
            glMultiTexCoord2d(GL_TEXTURE1_ARB, 0, 0);		
            glVertex2f(0, 0);
            
            glMultiTexCoord2d(GL_TEXTURE0_ARB, renderFboRect.width, 0);
            glMultiTexCoord2d(GL_TEXTURE1_ARB, renderFboRect.width, 0);		
            glVertex2f(renderFboRect.width, 0);
            
            glMultiTexCoord2d(GL_TEXTURE0_ARB, renderFboRect.width, renderFboRect.height);
            glMultiTexCoord2d(GL_TEXTURE1_ARB, renderFboRect.width, renderFboRect.height);
            glVertex2f(renderFboRect.width, renderFboRect.height);
            
            glMultiTexCoord2d(GL_TEXTURE0_ARB, 0, renderFboRect.height);
            glMultiTexCoord2d(GL_TEXTURE1_ARB, 0, renderFboRect.height);		
            glVertex2f(0, renderFboRect.height);
            
            glEnd();

            
            //deactive and clean up
            glActiveTexture(GL_TEXTURE1_ARB);
            dofBuffer.getTextureReference().unbind();
            
            glActiveTexture(GL_TEXTURE0_ARB);
            fbo1.getTextureReference().unbind();
            
            dofBlur.end();

            ofPopStyle();
            
            fbo2.end();     
            
            fbo1.begin();
            ofClear(0.0,0.0,0.0,0.0);
            
            ofPushStyle();
            ofEnableBlendMode(blendMode);
            
            ofSetColor(255, 255, 255, 255);
            dofBlur.begin();
            
            //mulit-text
            //our shader uses two textures, the top layer and the alpha
            //we can load two textures into a shader using the multi texture coordinate extensions
            glActiveTexture(GL_TEXTURE0_ARB);
            fbo2.getTextureReference().bind();
            
            glActiveTexture(GL_TEXTURE1_ARB);
            dofBuffer.getTextureReference().bind();
                    
            dofBlur.setUniform2f("sampleOffset", 0, dofBlurAmount);
            glBegin(GL_QUADS);
            
            //move the mask around with the mouse by modifying the texture coordinates
            glMultiTexCoord2d(GL_TEXTURE0_ARB, 0, 0);
            glMultiTexCoord2d(GL_TEXTURE1_ARB, 0, 0);		
            glVertex2f(0, 0);
            
            glMultiTexCoord2d(GL_TEXTURE0_ARB, renderFboRect.width, 0);
            glMultiTexCoord2d(GL_TEXTURE1_ARB, renderFboRect.width, 0);		
            glVertex2f(renderFboRect.width, 0);
            
            glMultiTexCoord2d(GL_TEXTURE0_ARB, renderFboRect.width, renderFboRect.height);
            glMultiTexCoord2d(GL_TEXTURE1_ARB, renderFboRect.width, renderFboRect.height);
            glVertex2f(renderFboRect.width, renderFboRect.height);
            
            glMultiTexCoord2d(GL_TEXTURE0_ARB, 0, renderFboRect.height);
            glMultiTexCoord2d(GL_TEXTURE1_ARB, 0, renderFboRect.height);		
            glVertex2f(0, renderFboRect.height);
            
            glEnd();
            
            //deactive and clean up
            glActiveTexture(GL_TEXTURE1_ARB);
            dofBuffer.getTextureReference().unbind();
            
            glActiveTexture(GL_TEXTURE0_ARB);
            fbo2.getTextureReference().unbind();
            
            dofBlur.end();
            
            ofPopStyle();
            
            fbo1.end();
        }


        rendererDirty = false;
	}
    
    ofPushStyle();
    ofSetColor(0);
    ofRect(fboRectangle);
    ofPopStyle();
    
    ofEnableAlphaBlending();
    fbo1.getTextureReference().draw(fboRectangle);
    

    
    rendererDirty = false;
    
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
	
    if(key == 'S'){
        loadShaders();
    }
//    if(key == '2'){
//        if( &renderer.getRGBTexture() == lowResPlayer){
//            renderer.setRGBTexture(*hiResPlayer);                
//        }
//        else{
//            renderer.setRGBTexture(*lowResPlayer);    
//        }
//    }
	
	if(key == '\t'){
		videoTimelineElement.toggleThumbs();
		depthSequence.toggleThumbs();
	}
    
    if(key == '1'){
        gui.setPage(1);
    }
    else if(key == '2'){
        gui.setPage(2);
    }
    else if(key == '3'){
        gui.setPage(3);
    }
    else if(key == '4'){
        gui.setPage(4);
    }
    else if(key == '5'){
        gui.setPage(5);
    }
    else if(key == '6'){
        gui.setPage(6);
    }

    if(key == '!'){
        timeline.setCurrentPage(0);
    }
    else if(key == '@'){
        timeline.setCurrentPage(1);
    }
    else if(key == '#'){
        timeline.setCurrentPage(2);
    }
    else if(key == '$'){
        timeline.setCurrentPage(3);
    }
    else if(key == '%'){
        timeline.setCurrentPage(4);
    }
    else if(key == '^'){
        timeline.setCurrentPage(5);
    }
    else if(key == '&'){
        timeline.setCurrentPage(6);
    }
    else if(key == '*'){
        timeline.setCurrentPage(7);
    }
    else if(key == '('){
        timeline.setCurrentPage(8);
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
	rendererDirty = true; //catch all for buttons...
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
	
    if(shouldExportSettings){
        
        settingsExporter.mirror = currentMirror;
        settingsExporter.zThreshold = timeline.getKeyframeValue("Z Threshold");
        settingsExporter.edgeClip = timeline.getKeyframeValue("Edge Snip");
        
        settingsExporter.offset = ofVec2f(currentXMultiplyShift, currentYMultiplyShift);
//        settingsExporter.simplify = currentSimplify;
        settingsExporter.simplify = renderer.getSimplification();
        settingsExporter.startFrame = timeline.getInFrame();
        settingsExporter.endFrame = timeline.getOutFrame();
        
        settingsExporter.drawPointcloud = drawPointcloud;
        settingsExporter.drawWireframe = drawWireframe;
        
        settingsExporter.wireFrameSize = timeline.getKeyframeValue("Wireframe Thickness");
        settingsExporter.pointSize = timeline.getKeyframeValue("Point Size");
        
        settingsExporter.fillHoles = fillHoles;
        settingsExporter.kernelSize = holeFiller.getKernelSize();
        settingsExporter.iterations = holeFiller.getIterations();
        
        settingsExporter.cameraPosition.setPosition(cam.getPosition());
        settingsExporter.cameraPosition.setOrientation(cam.getOrientationQuat());
		
        settingsExporter.saveToXml(currentCompositionDirectory + "SettingsExport.xml");
        cout << "saved settings to " << (currentCompositionDirectory + "SettingsExport.xml") << endl;
        shouldExportSettings = false;
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
//		ofSystemAlertDialog("This composition doesn't have a hi res movie so we can't render!");
//		startRenderMode = false;
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
		saveFolder = currentCompositionDirectory + "rendered" + pathDelim;
		ofDirectory outputDirectory(saveFolder);
		if(!outputDirectory.exists()){
	         outputDirectory.create(true);   
        }
        
        if(hasHiresVideo){
			hiResPlayer->play();
			hiResPlayer->setSpeed(0);
			hiResPlayer->setVolume(0);
            renderer.setRGBTexture(*hiResPlayer);
		}        
        
		lastRenderFrame = currentRenderFrame-1;
		numFramesToRender = timeline.getOutFrame() - timeline.getInFrame();
		numFramesRendered = 0;
		currentLockCamera = cameraTrack.lockCameraToTrack = true;
		cameraTrack.setTimelineInOutToTrack();
		currentRenderFrame = timeline.getInFrame();
	}
	
    
    /*--------------------------------------------------------------*
     update Video Players
     
     IF RENDERING
      - update high res if available
      - update low res if not
     
     ELSE
      - update low res ALWAYS
      - update high res if availabe
      - IF [ no temporal alignment + new Frame || temporal alignment and depth frame != selected depth frame from timeline]
            - updating renderer with low res data ALWAYS 
            (never give it high res, even if available)
     *--------------------------------------------------------------*/
    
	if(currentlyRendering){
		//hiResPlayer->setFrame(currentRenderFrame % hiResPlayer->getTotalNumFrames());
		timeline.setCurrentFrame(currentRenderFrame);
		videoTimelineElement.selectFrame(currentRenderFrame);
	    if(&renderer.getRGBTexture() == hiResPlayer){
			hiResPlayer->setFrame(videoTimelineElement.selectFrame(currentRenderFrame));
			hiResPlayer->update();
            updateRenderer(*hiResPlayer);		
        }
        else{
			lowResPlayer->setFrame(videoTimelineElement.selectFrame(currentRenderFrame));
			lowResPlayer->update();
            
            updateRenderer(*lowResPlayer);
            
        }
	}
	
	if(!currentlyRendering){
        lowResPlayer->update();
	    if(&renderer.getRGBTexture() == hiResPlayer){
            hiResPlayer->setFrame(lowResPlayer->getCurrentFrame());
            hiResPlayer->update();  
        }
        
		if(!temporalAlignmentMode && lowResPlayer->isFrameNew()){
			updateRenderer(*lowResPlayer);
		}
		//cout << "timeline is " << videoTimelineElement.getSelectedFrame() << " and player is " << lowResPlayer->getCurrentFrame() << endl;; 
		if(temporalAlignmentMode && (currentDepthFrame != depthSequence.getSelectedFrame())){
			updateRenderer(*lowResPlayer);
		}
    
		//what is this?
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
    renderer.farClip *= renderer.farClip;
	renderer.meshRotate.x = timeline.getKeyframeValue("X Rotate");
    renderer.meshRotate.y = timeline.getKeyframeValue("Y Rotate");
    renderer.meshRotate.z = timeline.getKeyframeValue("Z Rotate");
    int simplification = int( timeline.getKeyframeValue("Simplify") );

	if(renderer.getSimplification() != simplification){
    	renderer.setSimplification(simplification);
    }
    
    // if any variables are not current, update all of them, pass new 
    // parameters into the renderer
    
	if(currentXMultiplyShift != renderer.xmult ||
	   currentYMultiplyShift != renderer.ymult ||
	   currentMirror != renderer.mirror ||
	   fillHoles != holeFiller.enable ||
	   currentHoleKernelSize != holeFiller.getKernelSize() ||
       currentHoleFillIterations != holeFiller.getIterations())
	{		
		renderer.xmult = currentXMultiplyShift;
		renderer.ymult = currentYMultiplyShift;
		renderer.mirror = currentMirror;
		
		holeFiller.enable = fillHoles;
		holeFiller.setKernelSize(currentHoleKernelSize);
		currentHoleKernelSize = holeFiller.getKernelSize();
		holeFiller.setIterations(currentHoleFillIterations);
		currentHoleFillIterations = holeFiller.getIterations();
        
		updateRenderer(*lowResPlayer);
	}
	
    if(timeline.getUserChangedValue()){
    	updateRenderer(*lowResPlayer);
    }
    
	if(!temporalAlignmentMode && !currentlyRendering && lowResPlayer->getSpeed() == 0.0 && videoTimelineElement.getSelectedFrame() != timeline.getCurrentFrame()){
		videoTimelineElement.selectFrame(timeline.getCurrentFrame());
	}	

    //This section updates the faceTracker if the texture source has updated, and records the face tracker
    //features so that they can be pulled from an XML file to avoid computational overhead in the future.
    //If the features have already been recorded then we don't need to do anything here other than update
    //facePosition/Scale/Orientation/RotationMatrix.
    
    if(lowResPlayer->isFrameNew() && drawFaceTracker){
        
        faceTracker.update(toCv(*lowResPlayer));
           
        //facePosition = faceTracker.getPosition();
        //faceScale = faceTracker.getScale();
        //faceOrientation = faceTracker.getOrientation();
        //faceRotationMatrix = faceTracker.getRotationMatrix();
    }
    
    //updateFaceTrackerMesh(faceTracker, &faceTrackerMesh, renderer.getMesh());

    if(featureDraw){
        updateFDMesh();
    }

    
    /*--- Update Feature Detect Mesh ---*/
    if(featureDraw){
        updateFDMesh();
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

    //holeFilledPixels.setFromExternalPixels(depthSequence.currentDepthRaw.getPixels(), 640, 480, 1);
    //holeFiller.close(holeFilledPixels);
	holeFiller.close(depthSequence.currentDepthRaw);
    processDepthFrame();

    //renderer.setDepthImage(holeFilledPixels.getPixels());	
    renderer.setDepthImage(depthSequence.currentDepthRaw);	
	renderer.update();
	processGeometry();
	
	if(!drawPointcloud && !drawWireframe && !drawMesh){
		drawPointcloud = true;
	}	 
	
	currentDepthFrame = depthSequence.getSelectedFrame();

    rendererDirty = true;
}


//--------------------------------------------------------------
void testApp::updateFaceTrackerMesh(ofxFaceTracker fTracker, ofMesh* faceMesh, ofMesh rendererMesh)
{
    //vector<ofVec2f> texCoords = rendererMesh.getTexCoords();
    
    //for(int i = 0; i < rendererMesh.getNumTexCoords(); i++)
    //{
    //    cout << texCoords[i] << endl;
    //}
    /*
    float minDistance;
    int closestPointIndex;
    vector<ofVec2f> facePoints = fTracker.getImagePoints();
    
    if(fTracker.getFound()){
        
        for(int i = 0; i < facePoints.size(); i++){
            for(int j = 0; j < rendererMesh.getNumTexCoords(); j++){
                ofVec2f texCoord = rendererMesh.getTexCoord(j);
                
                if(j == 0){
                    minDistance = facePoints[i].squareDistance(texCoord);
                    closestPointIndex = 0;
                }
                else{
                    float newDist = facePoints[i].squareDistance(texCoord);
                    
                    if(minDistance > newDist){
                        minDistance = newDist;
                        closestPointIndex = j;
                    }
                }
            }
            faceMesh->addVertex(rendererMesh.getVertex(closestPointIndex));
        }
    }
    */ 
}




//--------------------------------------------------------------
void testApp::draw(){	
	ofBackground(255*.2);
    if(comps.size() == 0){
	    newCompButton->setPosAndSize(50, 50, 200, 200);	
    	saveCompButton->setPosAndSize(0, -200, 100, 25);//hidden
    }
    else{
        newCompButton->setPosAndSize(comps[0]->toggle->x, comps[0]->toggle->y-25, 100, 25);	
    }

	if(allLoaded){		
		if(!viewComps){
            
            //Here we are setting fboRectangle to maintain 16:9 ratio, then drawing within it using drawGeometry()
			//cout << timeline.getDrawRect().height << " tl height " << endl;
			if(!ofGetMousePressed(0)){
				timeline.setOffset(ofVec2f(0, ofGetHeight() - timeline.getDrawRect().height));
			}
            
			fboRectangle.height = (timeline.getDrawRect().y - fboRectangle.y - 20);
			fboRectangle.width = 16.0/9.0*fboRectangle.height;
			ofDrawBitmapString(currentCompositionDirectory + " -- " + lastSavedDate, ofPoint(fboRectangle.x, fboRectangle.y-15));
			
            float aspect = 1.0*fbo1.getWidth()/fbo1.getHeight();
			if(presentMode){
				fboRectangle.x = 0;
				fboRectangle.y = 0;
				fboRectangle.height = ofGetHeight();
				fboRectangle.width = aspect*fboRectangle.height;
			}
			else {
				fboRectangle.x = 250;
				fboRectangle.y = 100;
				fboRectangle.height = (timeline.getDrawRect().y - fboRectangle.y - 20);
				fboRectangle.width = aspect*fboRectangle.height;
				ofDrawBitmapString(currentCompositionDirectory, ofPoint(fboRectangle.x, fboRectangle.y-15));
			}
            newCompButton->setPosAndSize(fboRectangle.x+fboRectangle.width+25, 0, 100, 25);	
            saveCompButton->setPosAndSize(fboRectangle.x+fboRectangle.width+25, 25, 100, 25);

            drawGeometry();
            
            colorAlignAssistRect = ofRectangle(fboRectangle.x + fboRectangle.width,
                                               fboRectangle.y, fboRectangle.width/3,
                                               fboRectangle.height/3);
            float ratio = colorAlignAssistRect.width / lowResPlayer->getWidth();
            depthAlignAssistRect = ofRectangle(colorAlignAssistRect.x, 
                                               colorAlignAssistRect.y+colorAlignAssistRect.height,
                                               640 * ratio, 480 * ratio);

            vidAlignAssistRect = ofRectangle(colorAlignAssistRect.x, colorAlignAssistRect.y + 
                                             colorAlignAssistRect.height * 2 - 50, lowResPlayer->getWidth(), 
                                             lowResPlayer->getHeight());
     
            
			if(temporalAlignmentMode){
                lowResPlayer->draw(colorAlignAssistRect);
				depthSequence.currentDepthImage.draw(depthAlignAssistRect);
            }
            else{
                if(drawDOF){
                    dofBuffer.getTextureReference().draw(colorAlignAssistRect);
                }
                /*--- Draw Face Detection Small Vid ---*/
                if(drawFaceTracker){
                    
                    lowResPlayer->draw(vidAlignAssistRect.x, vidAlignAssistRect.y);
                    
                    if(faceTracker.getFound()){
                        ofPushMatrix();
                        ofSetLineWidth(3);
                        ofTranslate(vidAlignAssistRect.x, vidAlignAssistRect.y);
                        //ofScale(ratio, ratio, 1);
                        faceTracker.draw();
                        ofPopMatrix();
                        //ofSetupScreenOrtho(640, 360, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
                        //ofTranslate(700, 0);
                        //applyMatrix(faceRotationMatrix);
                        //ofScale(20,20,20); 
                        //ofDrawAxis(faceScale);
                    }
                }
                /*--- Draw Feature Detection Mesh Small Vid ---*/
                else if(featureDraw){
    
                        lowResPlayer->draw(vidAlignAssistRect.x, vidAlignAssistRect.y);
                        ofPushMatrix();
                        ofTranslate(vidAlignAssistRect.x, vidAlignAssistRect.y);
                        //if(hasHiresVideo)
                         //   ofScale(0.333,0.3333);
                        drawFDMesh(false);
                        ofPopMatrix();
                    
                }
            }
            
			if(currentlyRendering){
                ofFbo& fbo =  curbuf == 0 ? fbo1 : fbo2;
				fbo.getTextureReference().readToPixels(savingImage.getPixelsRef());
				char filename[512];
				sprintf(filename, "%s/save_%05d.png", saveFolder.c_str(), currentRenderFrame);
				savingImage.saveImage(filename);
				
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
	
    pauseRender = false;
    drawWireframe = true;

	saveComposition();
	refreshCompButtons();
	loadTimelineFromCurrentComp();
	
    viewComps = false;
	allLoaded = true;
    
	return true;
}

bool testApp::loadAssetsFromCompositionDirectory(string currentMediaFolder) {
	
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

	if(!loadAlignmentMatrices(calibrationDirectory)){
		ofSystemAlertDialog("Load Failed -- Couldn't Load Calibration Directory.");
		return false;
	}
	
	if(!loadVideoFile(videoPath, smallVideoPath)){
		ofSystemAlertDialog("Load Failed -- Couldn't load video.");
		return false;
	}
	
	if(!loadDepthSequence(depthImageDirectory)){
		ofSystemAlertDialog("Load Failed -- Couldn't load depth images.");
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
	
	depthPixelDecodeBuffer = depthSequence.currentDepthRaw.getPixels();
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
	
//	if(hasHiresVideo){
//		renderer.setTextureScale(1.0*lowResPlayer->getWidth()/hiResPlayer->getWidth(), 
//								 1.0*lowResPlayer->getHeight()/hiResPlayer->getHeight());
//	}
//	else { //refer to the calibration stats
//
//		float fullsizedWidth  = renderer.getRGBCalibration().getDistortedIntrinsics().getImageSize().width;
//		float fullsizedHeight = renderer.getRGBCalibration().getDistortedIntrinsics().getImageSize().height; 
////		cout << "image size is " << fullsizedWidth << " " << fullsizedHeight << endl;
//		renderer.setTextureScale(1.0*lowResPlayer->getWidth()/fullsizedWidth, 
//								 1.0*lowResPlayer->getHeight()/fullsizedHeight);
//	}
	
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
    
    if(!faceFilePresent){
        populatePointsFile(faceTrackerData, *lowResPlayer);
    }
    
	return true;
}

//--------------------------------------------------------------           
void testApp::populatePointsFile(ofxXmlSettings pointsFile, ofVideoPlayer player){
    pointsFile.addTag("recording");
    for(int i = 0; i < player.getTotalNumFrames(); i++){
        pointsFile.pushTag("recording");
        pointsFile.setValue("frame", i);
        pointsFile.popTag();
    }
    
    pointsFile.saveFile(currentCompositionDirectory + "trackingdata.xml");
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
    cout << "refreshing comp buttons" << endl;
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
//				comp->load->setup();
				comp->load->setDelegate(this);
				
				comp->toggle = new ofxMSAInteractiveObjectWithDelegate();
//				comp->toggle->setup();
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
    projectsettings.setValue("selfOcclude",selfOcclude);
	projectsettings.setValue("drawDOF",drawDOF);

	projectsettings.setValue("cameraSpeed", cam.speed);
	projectsettings.setValue("cameraRollSpeed", cam.rollSpeed);
	projectsettings.setValue("xmult", currentXMultiplyShift);
	projectsettings.setValue("ymult", currentYMultiplyShift);
	
    projectsettings.setValue("fillholes", fillHoles);
    projectsettings.setValue("kernelSize", currentHoleKernelSize);
    projectsettings.setValue("holeIterations", currentHoleFillIterations);
    
	projectsettings.setValue("pointcloud", drawPointcloud);
	projectsettings.setValue("wireframe", drawWireframe);
	projectsettings.setValue("mesh", drawMesh);
	projectsettings.setValue("mirror", currentMirror);
	projectsettings.setValue("duration", currentDuration);
	
	projectsettings.setValue("videoinout", enableVideoInOut);
	projectsettings.setValue("videoin", videoInPercent);
	projectsettings.setValue("videoout", videoOutPercent);
	
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
                cout << "selecting comp " << i << " for render " << comps[i]->batchExport << endl;
                
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
    drawDOF = projectsettings.getValue("drawDOF",true);
    
	selfOcclude = projectsettings.getValue("selfOcclude",false);

	currentXMultiplyShift = projectsettings.getValue("xmult", 0.);
	currentYMultiplyShift = projectsettings.getValue("ymult", 0.);
	
	drawPointcloud = projectsettings.getValue("pointcloud", false);
	drawWireframe = projectsettings.getValue("wireframe", false);
	drawMesh = projectsettings.getValue("mesh", false);
	currentMirror = projectsettings.getValue("mirror", false);
	currentDuration = projectsettings.getValue("duration", int(videoTimelineElement.videoThumbs.size()) );
	enableVideoInOut = projectsettings.getValue("videoinout", false);
	videoInPercent = projectsettings.getValue("videoin", 0.);
	videoOutPercent = projectsettings.getValue("videoout", 1.);
	
    fillHoles = projectsettings.getValue("fillholes", false);
    currentHoleKernelSize = projectsettings.getValue("kernelSize", 1);
    currentHoleFillIterations = projectsettings.getValue("holeIterations", 1);
	currentDuration = lowResPlayer->getTotalNumFrames();
	shouldResetDuration = true;
	
	//set keyframer files based on comp
	loadTimelineFromCurrentComp();

	//LOAD CAMERA SAVE AND POS
	cam.cameraPositionFile = currentCompositionDirectory + "camera_position.xml";
	cam.loadCameraPosition();
    

    accumulatedPerlinOffset = 0;
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
	renderer.setRGBTexture(*lowResPlayer);
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