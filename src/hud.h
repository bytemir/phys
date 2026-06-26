typedef struct {
    float objectDimension;
    float objectMass;
    bool ballMenuEnabled;
} HudProperties;

void RenderHud(void);
HudProperties *GetHudProperties(void);