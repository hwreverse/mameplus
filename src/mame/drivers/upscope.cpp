// license:BSD-3-Clause
// copyright-holders:Mariusz Wojcieszek
/**********************************************************************************

    Up Scope

    Driver by Mariusz Wojcieszek

***********************************************************************************

    Up Scope
    Grand Products 1986

    rom board       +--------------+
                                      amiga I/O board
    u4  u3  u2  u1
            u6  u5      Amiga 500      7404 7400
                                       74374 6116  8255  TIP120 TIP120
    u12 u11 u10 u9                         battery       TIP120 TIP120 TIP120 TIP120
            u14 u13                                      TIP120 TIP120 TIP120 TIP120



                    +---------------+

**********************************************************************************/

#include "emu.h"
#include "includes/amiga.h"
#include "cpu/m68000/m68000.h"
#include "machine/i8255.h"
#include "machine/nvram.h"
#include "machine/amigafdc.h"
#include "speaker.h"


class upscope_state : public amiga_state
{
public:
	upscope_state(const machine_config &mconfig, device_type type, const char *tag)
		: amiga_state(mconfig, type, tag),
	m_prev_cia1_porta(0xff),
	m_parallel_data(0xff),
	m_ppi(*this, "ppi")
	{ }

	uint8_t m_nvram[0x100];
	uint8_t m_prev_cia1_porta;
	uint8_t m_parallel_data;
	uint8_t m_nvram_address_latch;
	uint8_t m_nvram_data_latch;

	DECLARE_READ8_MEMBER(upscope_cia_0_portb_r);
	DECLARE_WRITE8_MEMBER(upscope_cia_0_portb_w);
	DECLARE_READ8_MEMBER(upscope_cia_1_porta_r);
	DECLARE_WRITE8_MEMBER(upscope_cia_1_porta_w);

	DECLARE_WRITE8_MEMBER(lamps_w);
	DECLARE_WRITE8_MEMBER(coin_counter_w);

	void init_upscope();

	void upscope(machine_config &config);
	void a500_mem(address_map &map);
	void main_map(address_map &map);
	void overlay_512kb_map(address_map &map);
protected:
	virtual void machine_reset() override;

private:
	required_device<i8255_device> m_ppi;
};


/*************************************
 *
 *  Debugging
 *
 *************************************/

#define LOG_IO          0



/*************************************
 *
 *  Reset state
 *
 *************************************/

void upscope_state::machine_reset()
{
	// reset base machine
	amiga_state::machine_reset();

	m_prev_cia1_porta = 0xff;
}


WRITE8_MEMBER( upscope_state::upscope_cia_0_portb_w )
{
	m_parallel_data = data;
}

READ8_MEMBER( upscope_state::upscope_cia_0_portb_r )
{
	return m_nvram_data_latch;
}



/*************************************
 *
 *  CIA-B port A access:
 *
 *  PA7 = com line /DTR
 *  PA6 = com line /RTS
 *  PA5 = com line /carrier detect
 *  PA4 = com line /CTS
 *  PA3 = com line /DSR
 *  PA2 = SEL (Centronics parallel control)
 *  PA1 = POUT (Centronics parallel control)
 *  PA0 = BUSY (Centronics parallel control)
 *
 *************************************/

READ8_MEMBER( upscope_state::upscope_cia_1_porta_r )
{
	return 0xf8 | (m_prev_cia1_porta & 0x07);
}

WRITE8_MEMBER( upscope_state::upscope_cia_1_porta_w )
{
	/* on a low transition of POUT, we latch stuff for the NVRAM */
	if ((m_prev_cia1_porta & 2) && !(data & 2))
	{
		/* if SEL == 1 && BUSY == 0, we latch an address */
		if ((data & 5) == 4)
		{
			if (LOG_IO) logerror("Latch address: %02X\n", m_parallel_data);
			m_nvram_address_latch = m_parallel_data;
		}

		/* if SEL == 1 && BUSY == 1, we write data to internal registers */
		else if ((data & 5) == 5)
		{
			m_ppi->write(space, m_nvram_address_latch & 0x03, m_parallel_data);
		}

		/* if SEL == 0 && BUSY == 1, we write data to NVRAM */
		else if ((data & 5) == 1)
		{
			if (LOG_IO) logerror("NVRAM data write @ %02X = %02X\n", m_nvram_address_latch, m_parallel_data);
			m_nvram[m_nvram_address_latch] = m_parallel_data;
		}

		/* if SEL == 0 && BUSY == 0, who knows? */
		else
		{
			logerror("Unexpected: POUT low with SEL == 0/BUSY == 0\n");
		}
	}

	/* on a low transition of BUSY, we latch stuff for reading */
	else if ((m_prev_cia1_porta & 1) && !(data & 1))
	{
		/* if SEL == 1, we read internal data registers */
		if (data & 4)
		{
			if (LOG_IO) logerror("Internal register (%d) read\n", m_nvram_address_latch);
			m_nvram_data_latch = m_ppi->read(space, m_nvram_address_latch & 0x03);
		}

		/* if SEL == 0, we read NVRAM */
		else
		{
			m_nvram_data_latch = m_nvram[m_nvram_address_latch];
			if (LOG_IO) logerror("NVRAM data read @ %02X = %02X\n", m_nvram_address_latch, m_nvram_data_latch);
		}
	}

	/* remember the previous value */
	m_prev_cia1_porta = data;
}

WRITE8_MEMBER( upscope_state::lamps_w )
{
	// 7-------  bubble light
	// -6------  sight
	// --5-----  torpedo 4
	// ---4----  torpedo 3
	// ----3---  torpedo 2
	// -----2--  torpedo 1
	// ------1-  enemy left
	// -------0  enemy right
}

WRITE8_MEMBER( upscope_state::coin_counter_w )
{
	machine().bookkeeping().coin_counter_w(0, data & 1);
}


/*************************************
 *
 *  Memory map
 *
 *************************************/

void upscope_state::overlay_512kb_map(address_map &map)
{
	map.unmap_value_high();
	map(0x000000, 0x07ffff).mirror(0x180000).ram().share("chip_ram");
	map(0x200000, 0x27ffff).rom().region("kickstart", 0);
}

void upscope_state::a500_mem(address_map &map)
{
	map.unmap_value_high();
	map(0x000000, 0x1fffff).m(m_overlay, FUNC(address_map_bank_device::amap16));
	map(0xa00000, 0xbfffff).rw(FUNC(upscope_state::cia_r), FUNC(upscope_state::cia_w));
	map(0xc00000, 0xd7ffff).rw(FUNC(upscope_state::custom_chip_r), FUNC(upscope_state::custom_chip_w));
	map(0xd80000, 0xddffff).noprw();
	map(0xde0000, 0xdeffff).rw(FUNC(upscope_state::custom_chip_r), FUNC(upscope_state::custom_chip_w));
	map(0xdf0000, 0xdfffff).rw(FUNC(upscope_state::custom_chip_r), FUNC(upscope_state::custom_chip_w));
	map(0xe00000, 0xe7ffff).nopw().r(FUNC(upscope_state::rom_mirror_r));
	map(0xe80000, 0xefffff).noprw(); // autoconfig space (installed by devices)
	map(0xf80000, 0xffffff).rom().region("kickstart", 0);
}

void upscope_state::main_map(address_map &map)
{
	a500_mem(map);
	map(0xf00000, 0xf7ffff).rom().region("user2", 0);
}



/*************************************
 *
 *  Input ports
 *
 *************************************/

static INPUT_PORTS_START( upscope )
	PORT_START("POT1DAT")
	PORT_BIT( 0xff, 0x80, IPT_PADDLE) PORT_SENSITIVITY(25) PORT_MINMAX(0x02,0xfe) PORT_KEYDELTA(5) PORT_CENTERDELTA(0) PORT_REVERSE

	PORT_START("IO0")
	PORT_SERVICE( 0x01, IP_ACTIVE_LOW )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_UP )
INPUT_PORTS_END



/*************************************
 *
 *  Machine driver
 *
 *************************************/

MACHINE_CONFIG_START(upscope_state::upscope)

	/* basic machine hardware */
	MCFG_DEVICE_ADD("maincpu", M68000, amiga_state::CLK_7M_NTSC)
	MCFG_DEVICE_PROGRAM_MAP(main_map)

	MCFG_DEVICE_ADD("overlay", ADDRESS_MAP_BANK, 0)
	MCFG_DEVICE_PROGRAM_MAP(overlay_512kb_map)
	MCFG_ADDRESS_MAP_BANK_ENDIANNESS(ENDIANNESS_BIG)
	MCFG_ADDRESS_MAP_BANK_DATA_WIDTH(16)
	MCFG_ADDRESS_MAP_BANK_ADDR_WIDTH(22)
	MCFG_ADDRESS_MAP_BANK_STRIDE(0x200000)

	MCFG_NVRAM_ADD_0FILL("nvram")

	/* video hardware */
	ntsc_video(config);

	MCFG_PALETTE_ADD("palette", 4096)
	MCFG_PALETTE_INIT_OWNER(upscope_state,amiga)

	MCFG_VIDEO_START_OVERRIDE(upscope_state,amiga)

	/* sound hardware */
	SPEAKER(config, "lspeaker").front_left();
	SPEAKER(config, "rspeaker").front_right();

	MCFG_DEVICE_ADD("amiga", PAULA_8364, amiga_state::CLK_C1_NTSC)
	MCFG_SOUND_ROUTE(0, "rspeaker", 0.50)
	MCFG_SOUND_ROUTE(1, "lspeaker", 0.50)
	MCFG_SOUND_ROUTE(2, "lspeaker", 0.50)
	MCFG_SOUND_ROUTE(3, "rspeaker", 0.50)
	MCFG_PAULA_MEM_READ_CB(READ16(*this, amiga_state, chip_ram_r))
	MCFG_PAULA_INT_CB(WRITELINE(*this, amiga_state, paula_int_w))

	/* cia */
	MCFG_DEVICE_ADD("cia_0", MOS8520, amiga_state::CLK_E_NTSC)
	MCFG_MOS6526_IRQ_CALLBACK(WRITELINE(*this, amiga_state, cia_0_irq))
	MCFG_MOS6526_PA_OUTPUT_CALLBACK(WRITE8(*this, amiga_state, cia_0_port_a_write))
	MCFG_MOS6526_PB_INPUT_CALLBACK(READ8(*this, upscope_state, upscope_cia_0_portb_r))
	MCFG_MOS6526_PB_OUTPUT_CALLBACK(WRITE8(*this, upscope_state, upscope_cia_0_portb_w))
	MCFG_DEVICE_ADD("cia_1", MOS8520, amiga_state::CLK_E_NTSC)
	MCFG_MOS6526_IRQ_CALLBACK(WRITELINE(*this, amiga_state, cia_1_irq))
	MCFG_MOS6526_PA_INPUT_CALLBACK(READ8(*this, upscope_state, upscope_cia_1_porta_r))
	MCFG_MOS6526_PA_OUTPUT_CALLBACK(WRITE8(*this, upscope_state, upscope_cia_1_porta_w))

	/* fdc */
	MCFG_DEVICE_ADD("fdc", AMIGA_FDC, amiga_state::CLK_7M_NTSC)
	MCFG_AMIGA_FDC_INDEX_CALLBACK(WRITELINE("cia_1", mos8520_device, flag_w))
	MCFG_AMIGA_FDC_READ_DMA_CALLBACK(READ16(*this, amiga_state, chip_ram_r))
	MCFG_AMIGA_FDC_WRITE_DMA_CALLBACK(WRITE16(*this, amiga_state, chip_ram_w))
	MCFG_AMIGA_FDC_DSKBLK_CALLBACK(WRITELINE(*this, amiga_state, fdc_dskblk_w))
	MCFG_AMIGA_FDC_DSKSYN_CALLBACK(WRITELINE(*this, amiga_state, fdc_dsksyn_w))

	// i/o extension
	MCFG_DEVICE_ADD("ppi", I8255, 0)
	MCFG_I8255_IN_PORTA_CB(IOPORT("IO0"))
	MCFG_I8255_OUT_PORTB_CB(WRITE8(*this, upscope_state, lamps_w))
	MCFG_I8255_OUT_PORTC_CB(WRITE8(*this, upscope_state, coin_counter_w))
MACHINE_CONFIG_END



/*************************************
 *
 *  ROM definitions
 *
 *************************************/

ROM_START( upscope )
	ROM_REGION(0x80000, "kickstart", 0)
	ROM_LOAD16_WORD_SWAP("315093-01.u2", 0x00000, 0x40000, CRC(a6ce1636) SHA1(11f9e62cf299f72184835b7b2a70a16333fc0d88))
	ROM_COPY("kickstart", 0x000000, 0x40000, 0x40000)

	ROM_REGION(0x080000, "user2", 0)
	ROM_LOAD16_BYTE( "upscope.u5",   0x000000, 0x008000, CRC(c109912e) SHA1(dcac9522e3c4818b2a02212b9173540fcf4bd463) )
	ROM_LOAD16_BYTE( "upscope.u13",  0x000001, 0x008000, CRC(9c8b071a) SHA1(69f9f8c17630ed568975e65dadc03213677a12dd) )
	ROM_LOAD16_BYTE( "upscope.u6",   0x010000, 0x008000, CRC(962f371e) SHA1(5682c62f34df2cc70f6125cf14203087670571db) )
	ROM_LOAD16_BYTE( "upscope.u14",  0x010001, 0x008000, CRC(1231bfc1) SHA1(f99adfabb01c1a15130f82f6a09d5458109a28bb) )

	ROM_LOAD16_BYTE( "upscope.u1",   0x040000, 0x008000, CRC(7a8de1fb) SHA1(30b87f07e0e0f66699402dffaeb0ca00c554f23e) )
	ROM_LOAD16_BYTE( "upscope.u9",   0x040001, 0x008000, CRC(5d16521e) SHA1(93e0a1644bd8adbb6f9fca6d4a252c11812c6ada) )
	ROM_LOAD16_BYTE( "upscope.u2",   0x050000, 0x008000, CRC(2089ef6b) SHA1(a12d87c8b368ffbadb556aca2e43e50348d34839) )
	ROM_LOAD16_BYTE( "upscope.u10",  0x050001, 0x008000, CRC(fbab44f5) SHA1(cd49f1f79e2181b3a9c40aebfba9d7c314dc909b) )

	ROM_LOAD16_BYTE( "upscope.u3",   0x060000, 0x008000, CRC(9b325528) SHA1(5bde1a42b62dd810843349ee9edf76e1c7521653) )
	ROM_LOAD16_BYTE( "upscope.u11",  0x060001, 0x008000, CRC(40e54449) SHA1(7d6ed97b87d74d80776cb682c78cd3b4a68633f4) )
	ROM_LOAD16_BYTE( "upscope.u4",   0x070000, 0x008000, CRC(6585ef1d) SHA1(b95e5e424266a50d4b63501278eb5d618fde5ba2) )
	ROM_LOAD16_BYTE( "upscope.u12",  0x070001, 0x008000, CRC(a909e388) SHA1(62acc30ab97d6a46a6d0782bb4ceb58061332724) )
ROM_END



/*************************************
 *
 *  Driver init
 *
 *************************************/

void upscope_state::init_upscope()
{
	m_agnus_id = AGNUS_HR_NTSC;
	m_denise_id = DENISE;

	// allocate nvram
	subdevice<nvram_device>("nvram")->set_base(m_nvram, sizeof(m_nvram));
}



/*************************************
 *
 *  Game drivers
 *
 *************************************/

GAME( 1986, upscope, 0, upscope, upscope, upscope_state, init_upscope, ORIENTATION_FLIP_X, "Grand Products", "Up Scope", MACHINE_IMPERFECT_SOUND )
