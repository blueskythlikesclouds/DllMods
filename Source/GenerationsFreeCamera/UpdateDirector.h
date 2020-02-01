#pragma once

void addUpdateCommand(void* context, float elapsedTime, const char* name);
void addRenderCommand(void* context, float elapsedTime, const char* name);
void addUpdateAndRenderCommand(void* context, float elapsedTime, const char* name);

void callUnknownFunction(void* context, float elapsedTime);

void* finishRenderer(void* context);
void* finishUpdater(void* context);