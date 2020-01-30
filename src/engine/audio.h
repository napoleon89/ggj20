#pragma once

struct SoundEffect {
	
	
	void loadFromMem() {
		
		
	}
};

struct AudioEngine {
	
	
	virtual void init();
	virtual void playSound();
	virtual void uninit();
};
