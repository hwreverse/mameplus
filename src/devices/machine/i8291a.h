// license:BSD-3-Clause
// copyright-holders: Sven Schnelle
/*********************************************************************

    i8291a.h

    I8291A GPIB controller

*********************************************************************/

#ifndef MAME_MACHINE_I8291A_H
#define MAME_MACHINE_I8291A_H


#pragma once


#define MCFG_I8291A_INT_WRITE_CB(_write)                               \
	downcast<i8291a_device &>(*device).set_int_write_cb(DEVCB_##_write);

#define MCFG_I8291A_DREQ_WRITE_CB(_write)                               \
	downcast<i8291a_device &>(*device).set_dreq_write_cb(DEVCB_##_write);

#define MCFG_I8291A_TRIG_WRITE_CB(_write)                               \
	downcast<i8291a_device &>(*device).set_trig_write_cb(DEVCB_##_write);

#define MCFG_I8291A_DIO_READ_CB(_read)             \
	downcast<i8291a_device &>(*device).set_dio_read_cb(DEVCB_##_read);

#define MCFG_I8291A_DIO_WRITE_CB(_write)                   \
	downcast<i8291a_device &>(*device).set_dio_write_cb(DEVCB_##_write);

#define MCFG_I8291A_EOI_WRITE_CB(_write)                               \
	downcast<i8291a_device &>(*device).set_eoi_write_cb(DEVCB_##_write);

#define MCFG_I8291A_DAV_WRITE_CB(_write)                               \
	downcast<i8291a_device &>(*device).set_dav_write_cb(DEVCB_##_write);

#define MCFG_I8291A_NRFD_WRITE_CB(_write)                              \
	downcast<i8291a_device &>(*device).set_nrfd_write_cb(DEVCB_##_write);

#define MCFG_I8291A_NDAC_WRITE_CB(_write)                              \
	downcast<i8291a_device &>(*device).set_ndac_write_cb(DEVCB_##_write);

#define MCFG_I8291A_SRQ_WRITE_CB(_write)                               \
	downcast<i8291a_device &>(*device).set_srq_write_cb(DEVCB_##_write);


class i8291a_device : public device_t
{
public:
	// construction/destruction
	i8291a_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	template <class Object> devcb_base& set_int_write_cb(Object &&cb)
		{ return m_int_write_func.set_callback(std::forward<Object>(cb)); }

	template <class Object> devcb_base& set_dreq_write_cb(Object &&cb)
		{ return m_dreq_write_func.set_callback(std::forward<Object>(cb)); }

	template <class Object> devcb_base& set_trig_write_cb(Object &&cb)
		{ return m_trig_write_func.set_callback(std::forward<Object>(cb)); }

	template <class Object> devcb_base& set_eoi_write_cb(Object &&cb)
		{ return m_eoi_write_func.set_callback(std::forward<Object>(cb)); }

	template <class Object> devcb_base& set_dav_write_cb(Object &&cb)
		{ return m_dav_write_func.set_callback(std::forward<Object>(cb)); }

	template <class Object> devcb_base& set_nrfd_write_cb(Object &&cb)
		{ return m_nrfd_write_func.set_callback(std::forward<Object>(cb)); }

	template <class Object> devcb_base& set_ndac_write_cb(Object &&cb)
		{ return m_ndac_write_func.set_callback(std::forward<Object>(cb)); }

	template <class Object> devcb_base& set_srq_write_cb(Object &&cb)
		{ return m_srq_write_func.set_callback(std::forward<Object>(cb)); }

	template <class Object> devcb_base& set_dio_write_cb(Object &&cb)
		{ return m_dio_write_func.set_callback(std::forward<Object>(cb)); }

	template <class Object> devcb_base& set_dio_read_cb(Object &&cb)
		{ return m_dio_read_func.set_callback(std::forward<Object>(cb)); }

	// Signal inputs
	DECLARE_WRITE_LINE_MEMBER(reset_w);
	DECLARE_WRITE_LINE_MEMBER(dack_w);

	// signal output
	devcb_write_line m_int_write_func;
	devcb_write_line m_dreq_write_func;
	devcb_write_line m_trig_write_func;
	devcb_write_line m_eoi_write_func;
	devcb_write_line m_dav_write_func;
	devcb_write_line m_nrfd_write_func;
	devcb_write_line m_ndac_write_func;
	devcb_write_line m_srq_write_func;

	devcb_write8 m_dio_write_func;
	devcb_read8 m_dio_read_func;

	void set_dav(bool state);
	void set_nrfd(bool state);
	void set_ndac(bool state);
	void set_eoi(bool state);
	void set_srq(bool state);

	// GPIB port
	DECLARE_WRITE_LINE_MEMBER(eoi_w);
	DECLARE_WRITE_LINE_MEMBER(dav_w);
	DECLARE_WRITE_LINE_MEMBER(nrfd_w);
	DECLARE_WRITE_LINE_MEMBER(ndac_w);
	DECLARE_WRITE_LINE_MEMBER(ifc_w);
	DECLARE_WRITE_LINE_MEMBER(srq_w);
	DECLARE_WRITE_LINE_MEMBER(atn_w);
	DECLARE_WRITE_LINE_MEMBER(ren_w);
	DECLARE_WRITE8_MEMBER(dio_w);

	// register r/w functions

	DECLARE_WRITE8_MEMBER(dout_w);
	DECLARE_WRITE8_MEMBER(ie1_w);
	DECLARE_WRITE8_MEMBER(ie2_w);
	DECLARE_WRITE8_MEMBER(spoll_mode_w);
	DECLARE_WRITE8_MEMBER(addr_mode_w);
	DECLARE_WRITE8_MEMBER(aux_mode_w);
	DECLARE_WRITE8_MEMBER(addr01_w);
	DECLARE_WRITE8_MEMBER(eos_w);

	DECLARE_READ8_MEMBER(din_r);
	DECLARE_READ8_MEMBER(ints1_r);
	DECLARE_READ8_MEMBER(ints2_r);
	DECLARE_READ8_MEMBER(spoll_stat_r);
	DECLARE_READ8_MEMBER(addr_stat_r);
	DECLARE_READ8_MEMBER(cpt_r);
	DECLARE_READ8_MEMBER(addr0_r);
	DECLARE_READ8_MEMBER(addr1_r);
	void map(address_map &map);

private:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	void update_int(void);
	void handle_command(void);
	void run_fsm();
	void run_sh_fsm();
	void run_ah_fsm();
	void run_dt_fsm();
	void run_dc_fsm();
	void run_l_fsm();
	void run_lp_fsm();
	void run_t_fsm();
	void run_tp_fsm();
	void run_tsp_fsm();
	void run_sp_fsm();
	void run_pp_fsm();
	void run_rl_fsm();

	// registers

	uint8_t m_din;
	uint8_t m_dout;
	uint8_t m_ints1;
	uint8_t m_ints2;
	uint8_t m_ie1;
	uint8_t m_ie2;
	uint8_t m_address0;
	uint8_t m_address1;
	uint8_t m_eos;
	uint8_t m_spoll_mode;
	uint8_t m_address_mode;
	uint8_t m_address_status;
	uint8_t m_cpt;

	uint8_t m_auxa;
	uint8_t m_auxb;

	bool m_atn;
	bool m_ren;
	bool m_nrfd;
	bool m_ndac;
	bool m_dav;
	bool m_srq;
	bool m_ifc;
	bool m_eoi;
	uint8_t m_dio;

	bool m_nrfd_out;
	bool m_ndac_out;
	bool m_dav_out;
	bool m_srq_out;
	bool m_eoi_out;

	// internal signals

	bool m_pon;
	bool m_rdy;
	bool m_lpe;
	bool m_ist;
	bool m_rtl;
	bool m_apt_flag;
	bool m_cpt_flag;
	bool m_din_flag;
	bool m_nba;

	bool m_pp_sense;
	bool m_pp_line;

	bool m_send_eoi;

	static constexpr int REG_INTS1_BI = (1 << 0);
	static constexpr int REG_INTS1_BO = (1 << 1);
	static constexpr int REG_INTS1_ERR = (1 << 2);
	static constexpr int REG_INTS1_DEC = (1 << 3);
	static constexpr int REG_INTS1_END = (1 << 4);
	static constexpr int REG_INTS1_GET = (1 << 5);
	static constexpr int REG_INTS1_APT = (1 << 6);
	static constexpr int REG_INTS1_CPT = (1 << 7);

	static constexpr int REG_INTS2_ADSC = (1 << 0);
	static constexpr int REG_INTS2_REMC = (1 << 1);
	static constexpr int REG_INTS2_LLOC = (1 << 2);
	static constexpr int REG_INTS2_SPC = (1 << 3);
	static constexpr int REG_INTS2_REM = (1 << 4);
	static constexpr int REG_INTS2_LLO = (1 << 5);
	static constexpr int REG_INTS2_SPAS = (1 << 6);
	static constexpr int REG_INTS2_INT = (1 << 7);

	static constexpr int REG_IE2_DMAI = (1 << 4);
	static constexpr int REG_IE2_DMAO = (1 << 5);

	static constexpr int REG_ADDRESS01_ARS = (1 << 7);
	static constexpr int REG_ADDRESS0_INT = (1 << 7);
	static constexpr int REG_ADDRESS_DT = (1 << 6);
	static constexpr int REG_ADDRESS_DL = (1 << 5);

	static constexpr int REG_ADDRESS_STATUS_MJMN = 1 << 0;
	static constexpr int REG_ADDRESS_STATUS_TA = 1 << 1;
	static constexpr int REG_ADDRESS_STATUS_LA = 1 << 2;
	static constexpr int REG_ADDRESS_STATUS_TPAS = 1 << 3;
	static constexpr int REG_ADDRESS_STATUS_LPAS = 1 << 4;
	static constexpr int REG_ADDRESS_STATUS_EOI = 1 << 5;
	static constexpr int REG_ADDRESS_STATUS_LON = 1 << 6;
	static constexpr int REG_ADDRESS_STATUS_TON = 1 << 7;

	static constexpr int REG_AUXB_CPT_ENABLE = 1 << 0;
	static constexpr int REG_AUXB_EOI_SPAS_ENABLE = 1 << 1;
	static constexpr int REG_AUXB_HS_TRANSFER = 1 << 2;
	static constexpr int REG_AUXB_INT_ACTIVE_LOW = 1 << 3;
	static constexpr int REG_AUXB_RFD_HOLDOFF_GET_DEC = 1 << 4;

	static constexpr int REG_AUXA_RFD_HOLDOFF_DATA = 1 << 0;
	static constexpr int REG_AUXA_RFD_HOLDOFF_END = 1 << 1;
	static constexpr int REG_AUXA_END_ON_EOS = 1 << 2;
	static constexpr int REG_AUXA_EOI_ON_EOS = 1 << 3;
	static constexpr int REG_AUXA_EOS_8BIT = 1 << 4;

	// AUX CMDs
	static constexpr int AUXCMD_IMMEDIATE_EXEC_PON = 0;
	static constexpr int AUXCMD_CLEAR_PP = 1;
	static constexpr int AUXCMD_CHIP_RESET = 2;
	static constexpr int AUXCMD_FINISH_HANDSHAKE = 3;
	static constexpr int AUXCMD_TRIGGER = 4;
	static constexpr int AUXCMD_CLEAR_RTL = 5;
	static constexpr int AUXCMD_SEND_EOI = 6;
	static constexpr int AUXCMD_NON_VALID_SA = 7;
	static constexpr int AUXCMD_PON = 8;
	static constexpr int AUXCMD_SET_PP = 9;
	static constexpr int AUXCMD_SET_RTL = 13;
	static constexpr int AUXCMD_VALID_SA = 15;

	// Interface commands
	// TODO: stolen from tms9914, move to common header file. PHI also defines this

	static constexpr uint8_t IFCMD_MASK       = 0x7f;  // Mask of valid bits in if. commands
	static constexpr uint8_t IFCMD_ACG_MASK   = 0x70;  // Mask of ACG commands
	static constexpr uint8_t IFCMD_ACG_VALUE  = 0x00;  // Value of ACG commands
	static constexpr uint8_t IFCMD_UCG_MASK   = 0x70;  // Mask of UCG commands
	static constexpr uint8_t IFCMD_UCG_VALUE  = 0x10;  // Value of UCG commands
	static constexpr uint8_t IFCMD_GROUP_MASK = 0x60;  // Mask of group id
	static constexpr uint8_t IFCMD_LAG_VALUE  = 0x20;  // Value of LAG commands
	static constexpr uint8_t IFCMD_TAG_VALUE  = 0x40;  // Value of TAG commands
	static constexpr uint8_t IFCMD_SCG_VALUE  = 0x60;  // Value of SCG commands
	static constexpr uint8_t IFCMD_GTL        = 0x01;  // Go to local
	static constexpr uint8_t IFCMD_SDC        = 0x04;  // Selected device clear
	static constexpr uint8_t IFCMD_GET        = 0x08;  // Group execute trigger
	static constexpr uint8_t IFCMD_TCT        = 0x09;  // Take control
	static constexpr uint8_t IFCMD_LLO        = 0x11;  // Local lock-out
	static constexpr uint8_t IFCMD_DCL        = 0x14;  // Device clear
	static constexpr uint8_t IFCMD_SPE        = 0x18;  // Serial poll enable
	static constexpr uint8_t IFCMD_SPD        = 0x19;  // Serial poll disable
	static constexpr uint8_t IFCMD_UNL        = 0x3f;  // Unlisten
	static constexpr uint8_t IFCMD_UNT        = 0x5f;  // Untalk

	enum class SourceHandshake {
		SIDS,
		SGNS,
		SDYS,
		STRS
	};

	enum class AcceptorHandshake {
		AIDS,
		ANRS,
		ACRS,
		AWNS,
		ADYS,
		ACDS
	};

	enum class TalkerState {
		TIDS,
		TADS,
		SPAS,
		TACS
	};

	enum class TalkerPrimaryState {
		TPIS,
		TPAS
	};

	enum class ListenerPrimaryState {
		LPIS,
		LPAS
	};

	enum class TalkerSerialPollState {
		SPIS,
		SPMS
	};

	enum class SerialPollState {
		NPRS,
		SRQS,
		APRS
	};

	enum class ListenerState {
		LIDS,
		LADS,
		LACS
	};

	enum class RemoteLocalState {
		LOCS,
		REMS,
		RWLS,
		LWLS
	};

	enum class ParallelPollState {
		PPIS,
		PPSS,
		PPAS
	};

	enum class DeviceClearState {
		DCIS,
		DCAS
	};

	enum class DeviceTriggerState {
		DTIS,
		DTAS
	};

	const char *get_state_name(AcceptorHandshake state);
	const char *get_state_name(SourceHandshake state);
	const char *get_state_name(TalkerState state);
	const char *get_state_name(TalkerPrimaryState state);
	const char *get_state_name(TalkerSerialPollState state);
	const char *get_state_name(ListenerState state);
	const char *get_state_name(ListenerPrimaryState state);
	const char *get_state_name(DeviceClearState state);
	const char *get_state_name(DeviceTriggerState state);
	const char *get_state_name(ParallelPollState state);
	const char *get_state_name(SerialPollState state);
	const char *get_state_name(RemoteLocalState state);

	template<typename T> void update_state(T &name, T state, int timeout = 0)
	{
			if (name != state) {
				m_state_changed = true;
				//logerror("%s: %s -> %s\n", __FUNCTION__,
				//		get_state_name(name),
				//		get_state_name(state));
				name = state;
			}
	}

	SourceHandshake m_sh_state;
	AcceptorHandshake m_ah_state;
	TalkerState m_t_state;
	TalkerPrimaryState m_tp_state;
	TalkerSerialPollState m_tsp_state;
	ListenerState m_l_state;
	ListenerPrimaryState m_lp_state;
	RemoteLocalState m_rl_state;
	SerialPollState m_sp_state;
	ParallelPollState m_pp_state;
	DeviceClearState m_dc_state;
	DeviceTriggerState m_dt_state;
	bool m_state_changed;
	bool m_ignore_ext_signals;
	bool m_intr_out;
	bool m_dreq_out;
};

// device type definition
DECLARE_DEVICE_TYPE(I8291A, i8291a_device)

#endif // MAME_MACHINE_I8291A_H
