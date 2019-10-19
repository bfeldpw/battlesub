#ifndef COMMON_H
#define COMMON_H

#include <Magnum/GL/Mesh.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/TranslationRotationScalingTransformation2D.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Shaders/VertexColor.h>

#include "error_handler.h"
#include "message_handler.h"

using namespace Magnum;

typedef SceneGraph::Object<SceneGraph::TranslationRotationScalingTransformation2D> Object2D;
typedef SceneGraph::Scene<SceneGraph::TranslationRotationScalingTransformation2D> Scene2D;

static ErrorHandler   GlobalErrorHandler;
static MessageHandler GlobalMessageHandler;

#endif // COMMON_H
