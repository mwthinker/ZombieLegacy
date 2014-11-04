#ifndef BUILDINGSHADER_H
#define BUILDINGSHADER_H

#include "box2ddef.h"

#include <mw/opengl.h>
#include <mw/matrix.h>
#include <mw/shader.h>
#include <mw/color.h>

namespace zombie {

	class BuildingShader {
	public:
		BuildingShader();
		BuildingShader(std::string vShaderFile, std::string fShaderFile);

		void useGlShader() const;

		// Vertex buffer Attributes. ---------------------------
		
		void setGlVer2dCoordsA(const GLvoid* data) const;
		void setGlVer2dCoordsA(GLsizei stride, const GLvoid* data) const;		

		void setGlTexCoordsA(const GLvoid* data) const;
		void setGlTexCoordsA(GLsizei stride, const GLvoid* data) const;
		
		void setGlHeightA(const GLvoid* data) const;
		void setGlHeightA(GLsizei stride, const GLvoid* data) const;

		// Uniforms. -------------------------------------------

		void setGlProjectionMatrixU(const mw::Matrix44& matrix) const;
		void setGlModelMatrixU(const mw::Matrix44& matrix) const;
		void setGlGlobalCenterPositionU(Position position) const;

	private:
		mw::Shader shader_;
		
		int aVerIndex_;
		int aTexIndex_;
		int aHeightIndex_;
		
		int uProjIndex_;
		int uModelIndex_;
		int uGlobalCenterPosIndex_;
	};

} // Namespace zombie.

#endif // BUILDINGSHADER_H
