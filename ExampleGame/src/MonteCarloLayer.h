#pragma once
#include "include.h"

class MonteCarloLayer :
    public Layer
{
public:
    MonteCarloLayer();
    ~MonteCarloLayer();

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float deltaTime) override;
    virtual void OnFixedUpdate() override;
    virtual void OnEvent(Event& e) override;
    virtual void OnImGuiRender() override;

private:
    void RunMonteCarlo();
private:
    int m_Runs;
    int m_Count = 0;

    std::vector<Vector2f> m_RedPositions;
    std::vector<Vector2f> m_BluePositions;

    OrthogrpahicCameraController m_CameraController;
};

