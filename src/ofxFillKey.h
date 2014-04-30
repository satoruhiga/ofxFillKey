#pragma once

#include "ofMain.h"

class ofxFillKey
{
public:
	
	void setup(int width, int height, int num_sample = 1)
	{
		ofFbo::Settings s;
		s.width = width;
		s.height = height;
		s.internalformat = GL_RGBA16F;
		s.numSamples = num_sample;
		fbo.allocate(s);
		
		fbo.begin();
		ofClear(0, 0);
		fbo.end();
		
#define _S(src) #src
		{
			const char *fs = _S
			(
			 uniform sampler2DRect tex;
			 void main()
			 {
				 float a = texture2DRect(tex, gl_TexCoord[0].xy).a;
				 gl_FragColor = vec4(a, a, a, 1.0);
			 }
			 );
			
			alpha_shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fs);
			alpha_shader.linkProgram();
		}
		
		{
			const char *fs = _S
			(
			 uniform sampler2DRect tex;
			 void main()
			 {
				 vec4 c = texture2DRect(tex, gl_TexCoord[0].xy);
				 float a = c.a;
				 
				 if (a == 0.)
				 {
					 gl_FragColor = vec4(1.);
				 }
				 else
				 {
					 vec3 m = c.rgb / a;
					 gl_FragColor = vec4(m, 1.);
				 }
			 }
			 );
			
			matte_shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fs);
			matte_shader.linkProgram();
		}
#undef _S
	}
	
	void begin()
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		
		ofPushStyle();
		fbo.begin();
		ofClear(0, 0);
	}
	
	void end()
	{
		fbo.end();
		ofPopStyle();
		
		glPopAttrib();
	}
	
	void drawFill(float x, float y, float width = 0, float height = 0)
	{
		if (width == 0) width = fbo.getWidth();
		if (height == 0) height = fbo.getHeight();
		
		ofPushStyle();
		ofSetColor(255, 255);
		
		matte_shader.begin();
		matte_shader.setUniformTexture("tex", fbo.getTextureReference(), 1);
		fbo.draw(x, y, width, height);
		matte_shader.end();
		
		ofPopStyle();
	}
	
	void drawKey(float x, float y, float width = 0, float height = 0)
	{
		if (width == 0) width = fbo.getWidth();
		if (height == 0) height = fbo.getHeight();
		
		ofPushStyle();
		ofSetColor(255, 255);
		
		alpha_shader.begin();
		alpha_shader.setUniformTexture("tex", fbo.getTextureReference(), 1);
		fbo.draw(x, y, width, height);
		alpha_shader.end();
		
		ofPopStyle();
	}
	
	void draw(float x, float y, float width = 0, float height = 0)
	{
		if (width == 0) width = fbo.getWidth();
		if (height == 0) height = fbo.getHeight();
		
		fbo.draw(x, y, width, height);
	}
	
	static void setSeparateBlendMode()
	{
		int src, dst;
		glGetIntegerv(GL_BLEND_SRC, &src);
		glGetIntegerv(GL_BLEND_DST, &dst);
		glBlendFuncSeparate(src, dst, GL_SRC_ALPHA, GL_ONE);
	}
	
	float getWidth() { return fbo.getWidth(); }
	float getHeight() { return fbo.getHeight(); }
	
	ofFbo& getFbo() { return fbo; }
	
private:
	
	ofFbo fbo;
	
	ofShader matte_shader;
	ofShader alpha_shader;
};
