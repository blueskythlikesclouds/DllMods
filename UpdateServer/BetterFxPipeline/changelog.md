# Better FxPipeline v2.3.1

* Fixed the issue where particles were overly bright when both the "FXAA intensity" and "post processing on particles" options were enabled.

# Better FxPipeline v2.3

* Refactored the codebase.
* Added an enhanced motion blur shader toggleable through mod options. This shader uses more samples and looks smoother.
* Fixed stretching artifacts when using letterbox mode on non-16:9 screens.
* Fixed overly bright bloom on resolutions below 1280x720.
* Updated default values for certain options (e.g. "shadow type" and "post-processing on particles"). This does not affect existing users.
* Enforced directional shadows now disable lambert shadows and set ambient bias to 0, however manual override is still available in mod options.
* Scene effect options enforced through Better FxPipeline no longer reflect to SceneEffect.prm.xml files saved in Parameter Editor.
* Reimplemented "Force shadow casting for objects" to correctly work for previously unaffected objects.
* Better FxPipeline now includes a compatibility check for old FxPipeline Renderer mods/patches. If detected as active, these incompatible mods will prevent the game from starting.

# Better FxPipeline v2.2

* Fixed a problem in the vanilla game where the sky model would not be rendered correctly in planar reflections.
* Made 3D mode use regular shaders. (3D mode normally uses shaders that lack some lighting features.)
* Added options to set shadow quality and type through the configuration UI. (HedgeModManager shadow resolution codes are not supported anymore.)

# Better FxPipeline v2.1

* Fixed planar reflections not getting affected by resolution scaling.
* Fixed title screen getting squished on the edges.

# Better FxPipeline v2.0

* Updated to not use the old FxPipeline Renderer anymore. All of the additional features are now brought back to the vanilla Sonic Generations renderer. Thus, in addition, every single issue with FxPipeline Renderer is now fixed.

# Better FxPipeline v1.11

* Fixed UI getting frozen during loading.

# Better FxPipeline v1.10

* Added support for vanilla Sonic Generations bloom shader. You can change the bloom type through the configuration UI in HedgeModManager. (right click on mod -> Configure mod). Please make sure to read the description of the option.
* This option can also be overriden in SceneEffect.prm.xml. You can use Parameter Editor to change it under SceneEffect.prm.xml -> BloomStar -> Extra.

# Better FxPipeline v1.9

* Added support for Sonic Unleashed bloom shader. You can enable it through the configuration UI in HedgeModManager. (right click on mod -> Configure mod). Please make sure to read the description of the option first.

# Better FxPipeline v1.8

* Added support for FXAA (Fast Approximate Anti-Aliasing). You can enable it through the configuration UI in HedgeModManager. (right click on mod -> Configure mod)

# Better FxPipeline v1.7

* Improved compatibility with the Direct3D 9 Ex mod. Particles (when PostProcessingOnParticles is enabled) should no longer look broken in a few stages.
* Fixed the infamous FxPipeline loading screen bug.
* Added a config schema for configuring the mod through HedgeModManager UI. (right click on mod -> Configure mod)

# Better FxPipeline v1.6

* Fixed issue where shadows of transparent objects would flicker.
* Added option to make terrain cast real-time shadows. See BetterFxPipeline.ini for more information.

# Better FxPipeline v1.5

* Added support for custom ambient shadow bias values. See BetterFxPipeline.ini for more information.

# Better FxPipeline v1.4.1

* Added update server.

# Better FxPipeline v1.4

* Fixed yet even more transparency issues.

# Better FxPipeline v1.3

* Fixed even more transparency issues.
* Particles (like jumpball) should stop randomly disappearing now.

# Better FxPipeline v1.2

* Fixed most of transparency issues.
* The spinball models appear correctly now.
* Sky Sanctuary clouds appear again.

# Better FxPipeline v1.1

* Added option to force ignoring of final light color adjustment.
* Added support for shadow ambient bias and lambert shadows.
* Fixed materials not getting affected by eyelight.
* Fixed materials not getting affected by eyelight.

# Better FxPipeline v1.0

* Initial release.