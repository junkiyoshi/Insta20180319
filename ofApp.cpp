#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofBackground(39);
	ofSetWindowTitle("Insta");

	this->cap.open("D:\\video\\image8.mp4");
	this->cap_size = cv::Size(1280, 720);
	ofSetFrameRate(this->cap.get(CV_CAP_PROP_FPS));

	this->box2d.init();
	this->box2d.setGravity(0, 10);
	this->box2d.createBounds();
	this->box2d.setFPS(30);
	this->box2d.registerGrabbing();

	this->rect_size = 80;
	for (int x = 480; x < ofGetWidth() - 480; x += this->rect_size) {

		for (int y = 160; y <= ofGetHeight() - 160; y += this->rect_size) {

			this->rects.push_back(shared_ptr<ofxBox2dRect>(new ofxBox2dRect));
			this->rects.back().get()->setPhysics(1.0, 0.1, 1.0);
			this->rects.back().get()->setup(this->box2d.getWorld(), ofRandom(ofGetWidth()), ofRandom(ofGetHeight()), this->rect_size, this->rect_size);

			ofImage* image = new ofImage();
			image->allocate(this->rect_size, this->rect_size, OF_IMAGE_COLOR);
			this->images.push_back(image);

			cv::Mat frame = cv::Mat(cv::Size(image->getWidth(), image->getHeight()), CV_MAKETYPE(CV_8UC3, image->getPixels().getNumChannels()), image->getPixels().getData(), 0);
			this->frames.push_back(frame);

			cv::Rect rect = cv::Rect(x, y, this->rect_size, this->rect_size);
			this->cv_rects.push_back(rect);
		}
	}

	this->edge = ofxBox2dEdge();
	this->edge.addVertex(ofVec2f(300, ofGetHeight() / 3 * 2));
	this->edge.addVertex(ofVec2f(ofGetWidth() - 300, ofGetHeight() / 3 * 2));
	this->edge.create(this->box2d.getWorld());

	this->fbo.allocate(ofGetWidth(), ofGetHeight());
}

//--------------------------------------------------------------
void ofApp::update() {

	cv::Mat src, frame;
	this->cap >> src;
	if (src.empty()) {

		cap.set(CV_CAP_PROP_POS_FRAMES, 1);
		return;
	}

	cv::resize(src, frame, this->cap_size);
	cv::cvtColor(frame, frame, CV_BGR2RGB);

	for (int i = 0; i < this->rects.size(); i++) {
	
		cv::Mat tmp_box_image(frame, this->cv_rects[i]);
		tmp_box_image.copyTo(this->frames[i]);
		this->images[i]->update();
	}

	this->box2d.update();

	this->fbo.begin();
	ofClear(0);

	for (int i = 0; i < this->rects.size(); i++) {

		ofPushMatrix();
		ofTranslate(this->rects[i].get()->getPosition().x, this->rects[i].get()->getPosition().y);
		ofRotate(this->rects[i].get()->getRotation());

		this->images[i]->draw(-40, -40);

		ofPopMatrix();
	}

	this->edge.draw();

	this->fbo.end();
}

//--------------------------------------------------------------
void ofApp::draw() {

	this->fbo.draw(0, 0);
}

//========================================================================
int main() {

	ofSetupOpenGL(1280, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}

