namespace salmon {
	template<typename T>
	struct cmpUnderlyingType {
		bool operator()(const T* a,
						const T* b) const {
			return *a < *b;
		}
	};
}
