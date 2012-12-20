#pragma once

#include "ofMain.h"

class ofxFillKey
{
public:
	
	void setup(int width, int height)
	{
		fbo.allocate(width, height, GL_RGBA);
		
#define _S(src) #src
		{
			const char *fs = _S
			(
			 uniform sampler2DRect tex;
			 void main()
			 {
				 float alpha_gain = 1.;
				 float a = texture2DRect(tex, gl_TexCoord[0].xy).a * alpha_gain;
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
				 a = clamp(a, 0., 1.);
				 vec3 m = vec3(1., 1., 1.) * (1. - a);
				 vec3 s = c.rgb;
				 gl_FragColor = vec4(s + m, 1.);
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
		
		setSeparateBlendMode();
	}
	
	void end()
	{
		fbo.end();
		ofPopStyle();
		
		glPopAttrib();
	}
	
	void drawFill(int x = 0, int y = 0, int width = 0, int height = 0)
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
	
	void drawKey(int x = 0, int y = 0, int width = 0, int height = 0)
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
	
	void setSeparateBlendMode()
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
