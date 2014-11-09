#ifndef WATERSHADER_H
#define WATERSHADER_H

#include "box2ddef.h"

#include <mw/opengl.h>
#include <mw/matrix.h>
#include <mw/shader.h>
#include <mw/color.h>

namespace zombie {

	class WaterShader {
	public:
		WaterShader();
		WaterShader(std::string vShaderFile, std::string fShaderFile);

		void glUseProgram() const;

		// Vertex buffer Attributes. ---------------------------
		void setGlVer2dCoordsA(const GLvoid* data) const;
		void setGlVer2dCoordsA(GLsizei stride, const GLvoid* data) const;

		// Uniforms. -------------------------------------------
		void setGlProjectionMatrixU(const mw::Matrix44& matrix) const;
		void setGlModelMatrixU(const mw::Matrix44& matrix) const;
		void setGlGlobalCenterPositionU(Position position) const;
		
		void setGlColorU(float red, float green, float blue, float alpha = 1) const;
		void setGlColorU(const mw::Color& color) const;

		void setGlTimeU(float time) const;

	private:
		mw::Shader shader_;
		
		int aVerIndex_;
		int aTexIndex_;
		
		int uProjIndex_;
		int uModelIndex_;
		int uGlobalCenterPosIndex_;

		int uTime_;
	};

} // Namespace zombie.

#endif // WATERSHADER_H
