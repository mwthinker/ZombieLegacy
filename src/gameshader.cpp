#include "gameshader.h"

namespace zombie {

	GameShader::GameShader() {
		aVerIndex_ = -1;
		aTexIndex_ = -1;

		// Collect the vertex buffer uniforms indexes.
		uProjIndex_ = -1;
		uModelIndex_ = -1;
		uGlobalCenterPosIndex_ = -1;
		uGlobalPosIndex_ = -1;
		uLocalAngleIndex_ = -1;

		// Collect the fragment buffer uniforms indexes.
		uColorIndex_ = -1;
		uIsTexIndex_ = -1;
	}

	GameShader::GameShader(std::string vShaderFile, std::string fShaderFile) {
		shader_.bindAttribute("aVer");
		shader_.bindAttribute("aTex");
		shader_.loadAndLinkFromFile(vShaderFile, fShaderFile);
		
		shader_.glUseProgram();

		// Collect the vertex buffer attributes indexes.
		aVerIndex_ = shader_.getAttributeLocation("aVer");
		aTexIndex_ = shader_.getAttributeLocation("aTex");

		// Collect the vertex buffer uniforms indexes.
		uProjIndex_ = shader_.getUniformLocation("uProj");
		uModelIndex_ = shader_.getUniformLocation("uModel");
		uGlobalCenterPosIndex_ = shader_.getUniformLocation("uGlobalCenterPos");
		uGlobalPosIndex_ = shader_.getUniformLocation("uGlobalPos");
		uLocalAngleIndex_ = shader_.getUniformLocation("uLocalAngle");
		
		// Collect the fragment buffer uniforms indexes.
		uColorIndex_ = shader_.getUniformLocation("uColor");
		uIsTexIndex_ = shader_.getUniformLocation("uIsTexture");
	}

	void GameShader::useGlShader() const {
		shader_.glUseProgram();
	}

	// Vertex buffer Attributes. ---------------------------

	void GameShader::setGlVer2dCoordsA(const GLvoid* data) const {
		mw::glVertexAttribPointer(aVerIndex_, 2, GL_FLOAT, GL_FALSE, 0, data);
		mw::glEnableVertexAttribArray(aVerIndex_);
	}

	void GameShader::setGlVer2dCoordsA(GLsizei stride, const GLvoid* data) const {
		mw::glVertexAttribPointer(aVerIndex_, 2, GL_FLOAT, GL_FALSE, stride, data);
		mw::glEnableVertexAttribArray(aVerIndex_);
	}

	void GameShader::setGlTexCoordsA(const GLvoid* data) const {
		mw::glVertexAttribPointer(aTexIndex_, 2, GL_FLOAT, GL_FALSE, 0, data);
		mw::glEnableVertexAttribArray(aTexIndex_);
	}

	void GameShader::setGlTexCoordsA(GLsizei stride, const GLvoid* data) const {
		mw::glVertexAttribPointer(aTexIndex_, 2, GL_FLOAT, GL_FALSE, stride, data);
		mw::glEnableVertexAttribArray(aTexIndex_);
	}	

	// Uniforms. -------------------------------------------

	void GameShader::setGlProjectionMatrixU(const mw::Matrix44& matrix) const {
		mw::glUniformMatrix4fv(uProjIndex_, 1, false, matrix.data());
	}

	void GameShader::setGlModelMatrixU(const mw::Matrix44& matrix) const {
		mw::glUniformMatrix4fv(uModelIndex_, 1, false, matrix.data());
	}

	void GameShader::setGlGlobalCenterPositionU(Position position) const {
		mw::glUniform2f(uGlobalCenterPosIndex_, position.x, position.y);
	}

	void GameShader::setGlGlobalPositionU(Position position) const {
		mw::glUniform2f(uGlobalPosIndex_, position.x, position.y);
	}

	void GameShader::setGlLocalAngleU(float angle) const {
		mw::glUniform1f(uLocalAngleIndex_, angle);
	}
	
	void GameShader::setGlColorU(float red, float green, float blue, float alpha) const {
		mw::glUniform4f(uColorIndex_, red, green, blue, alpha);
	}

	void GameShader::setGlColorU(const mw::Color& color) const {
		mw::glUniform4f(uColorIndex_, color.red_, color.green_, color.blue_, color.alpha_);
	}	

	void GameShader::setGlTextureU(bool texture) const {
		if (texture) {
			mw::glUniform1f(uIsTexIndex_, 1);
		} else {
			mw::glUniform1f(uIsTexIndex_, 0);
		}
	}

} // Namespace zombie.
