#pragma once

namespace audio {
	namespace synth {
		namespace filters {

			class Filter : public Component {
			public:
				virtual int Initialize() = 0;
				virtual int Update() = 0;
				virtual real_t Tick(real_t) = 0;
			};
		}
	}
}