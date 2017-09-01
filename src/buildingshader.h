#ifndef BUILDINGSHADER_H
#define BUILDINGSHADER_H

#include "box2ddef.h"

#include <mw/opengl.h>
#include <mw/shader.h>
#include <mw/color.h>

namespace zombie {

	class BuildingShader {
	public:
		BuildingShader();
		BuildingShader(std::string vShaderFile, std::string fShaderFile);

		void glUseProgram() const;

		// Vertex buffer Attributes. ---------------------------
		
		void setGlVer3dCoordsA(const GLvoid* data) const;
		void setGlVer3dCoordsA(GLsizei stride, const GLvoid* data) const;		

		void setGlTexCoordsA(const GLvoid* data) const;
		void setGlTexCoordsA(GLsizei stride, const GLvoid* data) const;

		// Uniforms. -------------------------------------------

		void setGlProjectionMatrixU(const Matrix44& matrix) const;
		void setGlModelMatrixU(const Matrix44& matrix) const;
		void setGlGlobalCenterPositionU(Position position) const;
		void setGlGlobalHumanPositionU(Position position) const;

	private:
		mw::Shader shader_;
		
		int aVerIndex_;
		int aTexIndex_;
		
		int uProjIndex_;
		int uModelIndex_;
		int uGlobalCenterPosIndex_;
		int uGlobalHumanPosIndex_;
	};

} // Namespace zombie.

#endif // BUILDINGSHADER_H
