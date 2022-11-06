#pragma once

#include "components/simple_scene.h"


namespace m1 
{
	class Tema1 : public gfxc::SimpleScene 
	{
		public:
			Tema1();
			~Tema1();

			void Init() override;

		private:
			void FrameStart() override;
			void Update(float deltaTimeSeconds) override;
			void FrameEnd() override;

			void OnInputUpdate(float deltaTime, int mods) override;
			void OnKeyPress(int key, int mods) override;
			void OnKeyRelease(int key, int mods) override;
			void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
			void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
			void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
			void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
			void OnWindowResize(int width, int height) override;

			float GenRandFloat(float min, float max);
			void CalculateValues();
			void CalculateDirAngle();
			void RotateInRefToPoint(glm::vec3 point, glm::vec3 refPoint, float angle);
			void RenderDuck();
			void RenderEye();
			void RenderLine(int sense);
			void RenderCross();
			void RenderBeak();
			void RenderHead();
			void RenderBody();
			void RenderWing(int sense);
			void RenderGrass();

		protected:
			glm::ivec2 resolution;
			glm::mat3 modelMatrix, parentMatrix;
			float bodyLength, wingLength, headRadius, beakLength, eyeRadius, lineLength;
			glm::vec3 bodyScale, bodyCorner, bodyCenter;
			glm::vec3 wingScale, wingCorner, wingCenter;
			glm::vec3 headScale, headCenter;
			glm::vec3 beakScale, beakCorner, beakCenter;
			glm::vec3 eyeScale, eyeCenter;
			glm::vec3 lineScale, lineCorner, lineCenter;
			glm::vec3 grassScale;
			float tx, ty;
			float wingRotationAngle;
			int wingRotationSense;
			glm::vec3 spawnPoint;
			float dirAngle;
			std::string currState;
			float speed, speedInc;
			float currDuckTime;
			int lives, bullets;

	};

}
