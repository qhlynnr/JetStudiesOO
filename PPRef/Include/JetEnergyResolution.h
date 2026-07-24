#ifndef JetResolution_h
#define JetResolution_h

namespace edm {
    namespace errors {
        enum ErrorCode {
            NotFound = 8026,
            ConfigFileReadError = 7002,
            UnimplementedFeature = 8011,
            FileReadError = 8021
        };
    };
};

class JetResolution 
{
    private:
    std::shared_ptr<JetResolutionObject> m_object;

    public:
        JetResolution(const std::string& filename);
        JetResolution(const JetResolutionObject& object);
        JetResolution(){}

        float getResolution(const JetParameters& parameters) const;
        void dump() const { m_object->dump();}

    };

    class JetResolutionScaleFactor {
        public:
            JetResolutionScaleFactor(const std::string& filename);
            JetResolutionScaleFactor(const JetResolutionObject& object);
            JetResolutionScaleFactor() {}

            float getScaleFactor(const JetParameters& parameters, Variation variation = Variation::NOMINAL) const;

            void dump() const { m_object->dump();}
    };

JetResolution::JetResolution(const std::string& filename) {
    m_object = std::make_shared<JetResolutionObject>(filename);
}

JetResolution::JetResolution(const JetResolutionObject& object) {
    m_object = std::make_shared<JetResolutionObject>(object);
}

float JetResolution::getResolution(const JetParameters& parameters) const {
    const JetResolutionObject::Record* record = m_object->getRecord(parameters);
        if (! record)
            return 1;

        return m_object->evaluateFormula(*record, parameters);
    }

    JetResolutionScaleFactor::JetResolutionScaleFactor(const std::string& filename) {
        m_object = std::make_shared<JetResolutionObject>(filename);
    }

    JetResolutionScaleFactor::JetResolutionScaleFactor(const JetResolutionObject& object) {
        m_object = std::make_shared<JetResolutionObject>(object);
    }

    float JetResolutionScaleFactor::getScaleFactor(const JetParameters& parameters, Variation variation/* = Variation::NOMINAL*/) const {
        const JetResolutionObject::Record* record = m_object->getRecord(parameters);
        if (! record)
            return 1;

        const std::vector<float>& parameters_values = record->getParametersValues();
        return parameters_values[static_cast<size_t>(variation)];
    }
#endif