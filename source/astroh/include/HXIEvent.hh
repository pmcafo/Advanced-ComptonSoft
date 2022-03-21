/** @file HXIEvent.hh
 * contains definitions of HXI event classes.
 *
 * @author Soki Sakurai
 * @author Hirokazu Odaka
 *
 * @date 2013-02-01 | S. Sakurai
 * @date 2015-02-08 | H. Odaka | Modification for ComptonSoft.
 * @date 2015-12-02 | H. Odaka | Review and all methods are redefined.
 *
 */

#ifndef ASTROH_HXI_EVENT_H
#define ASTROH_HXI_EVENT_H 1

#include <cstdint>
#include <cstddef>
#include <vector>
#include <iostream>
#include <iomanip>

namespace astroh {
namespace hxi {

constexpr std::size_t NumberOfASICs = 40;
constexpr std::size_t NumberOfChannelsInASIC = 32;
constexpr std::size_t NumberOfChannels = NumberOfASICs * NumberOfChannelsInASIC;
constexpr std::size_t MaxSizeOfRawASICDATA = 2000;


class EventFlags
{
public:
  explicit EventFlags(uint32_t flags=0)
    : flags_(flags)
  {
  }

  ~EventFlags() = default;

  EventFlags(const EventFlags&) = default;
  EventFlags(EventFlags&&) = default;
  EventFlags& operator=(const EventFlags&) = default;
  EventFlags& operator=(EventFlags&&) = default;

  void set(uint32_t flags)
  { flags_ = flags;}

  uint32_t get() const
  { return flags_; }
  
	// FLAG_FASTBGO in FITS
	uint8_t getFastBGO() const
  { return (flags_ & 0x3); }
  
	// FLAG_HITPAT in FITS
	uint8_t getHitPattern() const
  { return ((flags_ >> 2) & 0x3); }

	// FLAG_TRIGPAT in FITS
	uint8_t getTriggerPattern() const
  { return ((flags_ >> 4) & 0xff); }

	// FLAG_TRIG in FITS
	uint8_t getTrigger() const
  { return ((flags_ >> 12) & 0xff); }

	// FLAG_LCHK in FITS
	uint8_t getLengthCheck() const
  { return ((flags_ >> 20) & 0x1f); }

	//FLAG_SEU in FITS
	uint8_t getSEU() const
  { return ((flags_ >> 26) & 0x1f); }

private:
	uint32_t flags_;
};


class Event
{
public:
  static uint16_t makeASICID(uint16_t TrayNo, uint16_t ASICNo)
  {
    return (ASICNo<<4) + TrayNo;
  }

  static uint16_t getTrayNo(uint16_t ASIC_ID)
  {
    return ASIC_ID & 0xFu;
  }

  static uint16_t getASICNo(uint16_t ASIC_ID)
  {
    return (ASIC_ID>>4) & 0xFu;
  }

  static uint16_t convertToRemapedASICID(uint16_t ASIC_ID)
  {
    const uint16_t TrayNo = getTrayNo(ASIC_ID);
    const uint16_t ASICNo = getASICNo(ASIC_ID);
    const uint16_t RemappedID = TrayNo*8 + ASICNo + 1;
    return RemappedID;
  }

  static uint16_t makeRemappedReadoutID(uint16_t ASIC_ID, uint16_t readoutID)
  {
    const uint16_t TrayNo = getTrayNo(ASIC_ID);
    const uint16_t ASICNo = getASICNo(ASIC_ID);
    uint16_t channelRemapped(0u);
    if (ASICNo < 4) {
      channelRemapped = TrayNo*256 + (128 - ASICNo*32 - readoutID);
    }
    else {
      channelRemapped = TrayNo*256 + ASICNo*32 + (32-readoutID);
    }
    return channelRemapped;
  }
  
public:
  Event() = default;
  ~Event() = default;
  Event(const Event& ) = default;
  Event(Event&& ) = default;
  Event& operator=(const Event& ) = default;
  Event& operator=(Event&& ) = default;

  void initializeData()
  {
    // FFF contents
    TIME_ = 0.0;
    S_TIME_ = 0.0;
    ADU_CNT_ = 0;
    L32TI_ = 0;
    OCCURRENCE_ID_ = 0;
    LOCAL_TIME_ = 0;
    CATEGORY_ = 0;
    FLAGS_.set(0ul);
    LIVETIME_ = 0;
    NUM_ASIC_ = 0;
    RAW_ASIC_DATA_.clear();
    PROC_STATUS_ = 0;
    STATUS_ = 0;

    // SFF contents (ASICs)
    clearASICData();
    
    // SFF contents (readout channels)
    clearReadoutData();
  }

  // FFF contents
  void setTime(double v) { TIME_ = v; }
  double getTime() const { return TIME_; }

  void setSTime(double v) { S_TIME_ = v; }
  double getSTime() const { return S_TIME_; }

  void setADUCount(uint8_t v) { ADU_CNT_ = v; }
  uint8_t getADUCount() const { return ADU_CNT_; }
  
  void setL32TI(uint32_t v) { L32TI_ = v; }
  uint32_t getL32TI() const { return L32TI_; }

  void setOccurrenceID(int32_t v) { OCCURRENCE_ID_ = v; }
  int32_t getOccurrenceID() const { return OCCURRENCE_ID_; }
  
  void setLocalTime(uint32_t v) { LOCAL_TIME_ = v; }
  uint32_t getLocalTime() const { return LOCAL_TIME_; }

  void setCategory(uint8_t v) { CATEGORY_ = v; }
  uint8_t getCategory() const { return CATEGORY_; }

  void setFlags(EventFlags v) { FLAGS_ = v; }
  void setFlags(uint32_t v) { FLAGS_.set(v); }
  EventFlags getFlags() const { return FLAGS_; }

  void setLiveTime(uint32_t v) { LIVETIME_ = v; }
  uint32_t getLiveTime() const { return LIVETIME_; }

  void setNumberOfHitASICs(uint8_t v) { NUM_ASIC_ = v; }
  uint8_t getNumberOfHitASICs() const { return NUM_ASIC_; }

  void setRawASICData(const std::vector<uint8_t>& v)
  { RAW_ASIC_DATA_ = v; }
  void setRawASICData(std::vector<uint8_t>&& v)
  { RAW_ASIC_DATA_ = std::move(v); }
  const std::vector<uint8_t>& getRawASICData() const
  { return RAW_ASIC_DATA_; }

  void setProcessStatus(uint32_t v) { PROC_STATUS_ = v; }
  uint32_t getProcessStatus() const { return PROC_STATUS_; }

  void setStatus(uint8_t v) { STATUS_ = v; }
  uint8_t getStatus() const { return STATUS_; }

  // SFF contents (ASICs)
  void pushASICData(uint8_t ASIC_ID,
                    uint8_t ASIC_ID_remapped,
                    uint8_t chipDataBit,
                    uint8_t trigger,
                    uint8_t SEU,
                    uint32_t channelDataBit,
                    int16_t numberOfHitChannels,
                    int16_t referenceLevel,
                    int16_t commonModeNoise)
  {
    ASIC_ID_.push_back(ASIC_ID);
    ASIC_ID_RMAP_.push_back(ASIC_ID_remapped);
    ASIC_CHIP_.push_back(chipDataBit);
    ASIC_TRIG_.push_back(trigger);
    ASIC_SEU_.push_back(SEU);
    READOUT_FLAG_.push_back(channelDataBit);
    NUM_READOUT_.push_back(numberOfHitChannels);
    ASIC_REF_.push_back(referenceLevel);
    ASIC_CMN_.push_back(commonModeNoise);
  }

  void clearASICData()
  {
    ASIC_ID_.clear();
    ASIC_ID_RMAP_.clear();
    ASIC_CHIP_.clear();
    ASIC_TRIG_.clear();
    ASIC_SEU_.clear();
    READOUT_FLAG_.clear();
    NUM_READOUT_.clear();
    ASIC_REF_.clear();
    ASIC_CMN_.clear();
  }
    
  void reserveASICData(std::size_t size)
  {
    ASIC_ID_.reserve(size);
    ASIC_ID_RMAP_.reserve(size);
    ASIC_CHIP_.reserve(size);
    ASIC_TRIG_.reserve(size);
    ASIC_SEU_.reserve(size);
    READOUT_FLAG_.reserve(size);
    NUM_READOUT_.reserve(size);
    ASIC_REF_.reserve(size);
    ASIC_CMN_.reserve(size);
  }

  std::size_t LengthOfASICData() const
  {
    return ASIC_ID_.size();
  }

  const std::vector<uint8_t>& getASICIDVector() const
  { return ASIC_ID_; }

  const std::vector<uint8_t>& getASICIDRemappedVector() const
  { return ASIC_ID_RMAP_; }

  const std::vector<uint8_t>& getChipDataBitVector() const
  { return ASIC_CHIP_; }

  const std::vector<uint8_t>& getTriggerVector() const
  { return ASIC_TRIG_; }

  const std::vector<uint8_t>& getSEUVector() const
  { return ASIC_SEU_; }

  const std::vector<uint32_t>& getChannelDataBitVector() const
  { return READOUT_FLAG_; }

  const std::vector<int16_t>& getNumberOfHitChannelsVector() const
  { return NUM_READOUT_; }
  
  const std::vector<int16_t>& getReferenceLevelVector() const
  { return ASIC_REF_; }
  
  const std::vector<int16_t>& getCommonModeNoiseVector() const
  { return ASIC_CMN_; }

  // SFF contents (readout channels)
  void pushReadoutData(uint8_t ASIC_ID,
                       uint8_t channelID,
                       int16_t channelID_remapped,
                       int16_t PHA,
                       float EPI)
  {
    READOUT_ASIC_ID_.push_back(ASIC_ID);
    READOUT_ID_.push_back(channelID);
    READOUT_ID_RMAP_.push_back(channelID_remapped);
    PHA_.push_back(PHA);
    EPI_.push_back(EPI);
  }
    
  void clearReadoutData()
  {
    READOUT_ASIC_ID_.clear();
    READOUT_ID_.clear();
    READOUT_ID_RMAP_.clear();
    PHA_.clear();
    EPI_.clear();
  }
    
  void reserveReadoutData(std::size_t size)
  {
    READOUT_ASIC_ID_.reserve(size);
    READOUT_ID_.reserve(size);
    READOUT_ID_RMAP_.reserve(size);
    PHA_.reserve(size);
    EPI_.reserve(size);
  }

  std::size_t LengthOfReadoutData() const
  {
    return READOUT_ASIC_ID_.size();
  }

  const std::vector<uint8_t>& getReadoutASICIDVector() const
  { return READOUT_ASIC_ID_; }
  
  const std::vector<uint8_t>& getReadoutChannelIDVector() const
  { return READOUT_ID_; }
  
  const std::vector<int16_t>& getReadoutChannelIDRemappedVector() const
  { return READOUT_ID_RMAP_; }

  const std::vector<int16_t>& getPHAVector() const
  { return PHA_; }

  const std::vector<float>& getEPIVector() const
  { return EPI_; }

  void setEPI(std::size_t i, float v) { EPI_[i] = v; }
  float getEPI(std::size_t i) const { return EPI_[i]; }

  void output(std::ostream& os) const
  {
    std::ostream initialFormatter(nullptr);
    initialFormatter.copyfmt(os);

    os << std::showbase;
    os << std::dec;
    os << "TIME :            " << TIME_ << '\n';
    os << "S_TIME :          " << S_TIME_ << '\n';
    os << "ADU_CNT :         " << static_cast<int>(ADU_CNT_) << '\n';
    os << "L32TI :           " << L32TI_ << '\n';
    os << "OCCURRENCE_ID :   " << OCCURRENCE_ID_ << '\n';
    os << "LOCAL_TIME :      " << LOCAL_TIME_ << '\n';
    
    os << std::hex;
    os << "CATEGORY :        " << static_cast<int>(CATEGORY_) << '\n';
    os << "FLAGS :           " << FLAGS_.get() << '\n';
    os << "FLAG_SEU :        " << static_cast<int>(FLAGS_.getSEU()) << '\n';
    os << "FLAG_LCHK :       " << static_cast<int>(FLAGS_.getLengthCheck()) << '\n';
    os << std::dec;
    os << "FLAG_TRIG :       " << static_cast<int>(FLAGS_.getTrigger()) << '\n';
    os << std::hex;
    os << "FLAG_TRIGPAT :    " << static_cast<int>(FLAGS_.getTriggerPattern()) << '\n';
    os << "FLAG_HITPAT :     " << static_cast<int>(FLAGS_.getHitPattern()) << '\n';
    os << "FLAG_FASTBGO :    " << static_cast<int>(FLAGS_.getFastBGO()) << '\n';
    
    os << std::dec;
    os << "LIVETIME :        " << LIVETIME_ << '\n';
    os << "NUM_ASIC :        " << static_cast<int>(NUM_ASIC_) << '\n';
    os << "RAW_ASIC_DATA :   " ;
    for (auto a: RAW_ASIC_DATA_) os << static_cast<int>(a) << " ";
    os << '\n';
    os << "PROC_STATUS :     " << PROC_STATUS_ << '\n';
    os << "STATUS :          " << static_cast<int>(STATUS_) << '\n';
    os << "ASIC_ID :         " ;
    for (auto a: ASIC_ID_) os << static_cast<int>(a) << " ";
    os << '\n';
    os << "ASIC_ID_RMAP :    " ;
    for (auto a: ASIC_ID_RMAP_) os << static_cast<int>(a) << " ";
    os << '\n';
    os << "ASIC_CHIP :       " ;
    for (auto a: ASIC_CHIP_) os << static_cast<int>(a) << " ";
    os << '\n';
    os << "ASIC_TRIG :       " ;
    for (auto a: ASIC_TRIG_) os << static_cast<int>(a) << " ";
    os << '\n';
    os << "ASIC_SEU :        " ;
    for (auto a: ASIC_SEU_) os << static_cast<int>(a) << " ";
    os << '\n';
    os << "READOUT_FLAG :    " ;
    if (displayReadoutFlagSigned_) {
      for (auto a: READOUT_FLAG_) os << static_cast<int32_t>(a) << " ";
    }
    else {
      os << std::hex;
      for (auto a: READOUT_FLAG_) os << a << " ";
    }
    os << '\n';

    os << std::dec;
    os << "NUM_READOUT :     " ;
    for (auto a: NUM_READOUT_) os << static_cast<int>(a) << " ";
    os << '\n';
    os << "ASIC_REF :        " ;
    for (auto a: ASIC_REF_) os << static_cast<int>(a) << " ";
    os << '\n';
    os << "ASIC_CMN :        " ;
    for (auto a: ASIC_CMN_) os << static_cast<int>(a) << " ";
    os << '\n';
    os << "READOUT_ASIC_ID : " ;
    for (auto a: READOUT_ASIC_ID_) os << static_cast<int>(a) << " ";
    os << '\n';
    os << "READOUT_ID :      " ;
    for (auto a: READOUT_ID_) os << static_cast<int>(a) << " ";
    os << '\n';
    os << "READOUT_ID_RMAP : " ;
    for (auto a: READOUT_ID_RMAP_) os << static_cast<int>(a) << " ";
    os << '\n';
    os << "PHA :             " ;
    for (auto a: PHA_) os << a << " ";
    os << '\n';
    os << "EPI :             " ;
    for (auto a: EPI_) os << a << " ";
    
    os << std::endl;
    os.copyfmt(initialFormatter);
  }

private:
  // FFF contents
	double TIME_ = 0.0;
	double S_TIME_ = 0.0;
	uint8_t ADU_CNT_ = 0;
	uint32_t L32TI_ = 0;
	int32_t OCCURRENCE_ID_ = 0;
	uint32_t LOCAL_TIME_ = 0;
  uint8_t CATEGORY_ = 0;
  EventFlags FLAGS_;
	uint32_t LIVETIME_ = 0;
	uint8_t NUM_ASIC_ = 0;
	std::vector<uint8_t> RAW_ASIC_DATA_;
	uint32_t PROC_STATUS_ = 0;
  uint8_t STATUS_ = 0;

  // SFF contents (ASICs)
  std::vector<uint8_t> ASIC_ID_;
	std::vector<uint8_t> ASIC_ID_RMAP_;
	std::vector<uint8_t> ASIC_CHIP_;
	std::vector<uint8_t> ASIC_TRIG_;
	std::vector<uint8_t> ASIC_SEU_;
	std::vector<uint32_t> READOUT_FLAG_;
	std::vector<int16_t> NUM_READOUT_;
	std::vector<int16_t> ASIC_REF_;
	std::vector<int16_t> ASIC_CMN_;

  // SFF contents (readout channels)
  std::vector<uint8_t> READOUT_ASIC_ID_;
	std::vector<uint8_t> READOUT_ID_;
	std::vector<int16_t> READOUT_ID_RMAP_;
	std::vector<int16_t> PHA_;
	std::vector<float> EPI_;

  // display options
  bool displayReadoutFlagSigned_ = false;
};

} /* namespace hxi */
} /* namespace astroh */


inline
std::ostream& operator<<(std::ostream& os, const astroh::hxi::Event& event)
{
  event.output(os);
  return os;
}

#endif /* COMPTONSOFT_EVENT_H */
