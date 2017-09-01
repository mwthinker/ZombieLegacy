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
		
		shader_.useProgram();

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

	void GameShader::useProgram() const {
		shader_.useProgram();
	}

	// Vertex buffer Attributes. ---------------------------

	void GameShader::setGlVer2dCoordsA(const GLvoid* data) const {
		glVertexAttribPointer(aVerIndex_, 2, GL_FLOAT, GL_FALSE, 0, data);
		glEnableVertexAttribArray(aVerIndex_);
	}

	void GameShader::setGlVer2dCoordsA(GLsizei stride, const GLvoid* data) const {
		glVertexAttribPointer(aVerIndex_, 2, GL_FLOAT, GL_FALSE, stride, data);
		glEnableVertexAttribArray(aVerIndex_);
	}

	void GameShader::setGlTexCoordsA(const GLvoid* data) const {
		glVertexAttribPointer(aTexIndex_, 2, GL_FLOAT, GL_FALSE, 0, data);
		glEnableVertexAttribArray(aTexIndex_);
	}

	void GameShader::setGlTexCoordsA(GLsizei stride, const GLvoid* data) const {
		glVertexAttribPointer(aTexIndex_, 2, GL_FLOAT, GL_FALSE, stride, data);
		glEnableVertexAttribArray(aTexIndex_);
	}

	// Uniforms. -------------------------------------------

	void GameShader::setGlProjectionMatrixU(const Matrix44& matrix) const {
		glUniformMatrix4fv(uProjIndex_, 1, false, matrix.data());
	}

	void GameShader::setGlModelMatrixU(const Matrix44& matrix) const {
		glUniformMatrix4fv(uModelIndex_, 1, false, matrix.data());
	}

	void GameShader::setGlGlobalCenterPositionU(Position position) const {
		glUniform2f(uGlobalCenterPosIndex_, position.x, position.y);
	}

	void GameShader::setGlGlobalPositionU(Position position) const {
		glUniform2f(uGlobalPosIndex_, position.x, position.y);
	}

	void GameShader::setGlLocalAngleU(float angle) const {
		glUniform1f(uLocalAngleIndex_, angle);
	}
	
	void GameShader::setGlColorU(float red, float green, float blue, float alpha) const {
		glUniform4f(uColorIndex_, red, green, blue, alpha);
	}

	void GameShader::setGlColorU(const mw::Color& color) const {
		glUniform4f(uColorIndex_, color.red_, color.green_, color.blue_, color.alpha_);
	}	

	void GameShader::setGlTextureU(bool texture) const {
		if (texture) {
			glUniform1f(uIsTexIndex_, 1);
		} else {
			glUniform1f(uIsTexIndex_, 0);
		}
	}

} // Namespace zombie.
