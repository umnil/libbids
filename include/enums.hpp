#ifndef INCLUDE_ENUMS_HPP_
#define INCLUDE_ENUMS_HPP_
enum Modality { MRI, MEG, EEG, iEEG, PHYSIO, STIM };
inline bool is_primary(Modality const& m) {
  return (m != Modality::PHYSIO) && (m != Modality::STIM);
}
#endif /* INCLUDE_ENUMS_HPP_ */
