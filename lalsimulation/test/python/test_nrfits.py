#!/usr/bin/env python
#
# Copyright (C) 2020 Vijay Varma
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http: //www.gnu.org/licenses/>.

""" Regression tests for models in lalsimulation.nrfits.
    Currently implemented for NRSur7dq4Remnant and NRSur3dq8Remnant.
"""

import os
import sys
import pytest
import numpy as np

import lal
from lalsimulation.nrfits import eval_nrfit

# -- regression data ---------------------

# Format: (model_name, q, M, chiA_vec, chiB_vec, f_ref, mf, chif_vec, vf_vec)
# 10 cases each for NRSur7dq4Remnant and NRSur3dq8Remnant. Generated using:
# https://git.ligo.org/waveforms/reviews/RemnantFits/blob/master/generate_regression_data.py
test_data = [
    ('NRSur7dq4Remnant', 3.1015539805841841, 39.1090915951688132, [-0.0251951856245674, 0.0313291748557638, 0.6272539457964352], [-0.1258852023027293, 0.2037716949049488, -0.0076748106024794], 20.0000000000000000, 0.9568306149782949, [-0.0168236644301852, 0.0162984287699372, 0.8063127474299775], [-0.0000616419346058, -0.0001376382779657, -0.0000527753455964]),
    ('NRSur7dq4Remnant', 4.4286876981338654, 63.0273044045906659, [-0.0069180193425847, 0.0695193330872537, 0.2154410391088565], [0.0223373317673853, 0.0025044256058860, 0.0904504582739666], 20.0000000000000000, 0.9769684608678689, [-0.0015355883784699, 0.0360916215443596, 0.5676097203683161], [0.0003409665205972, 0.0001647631101576, 0.0001106076957332]),
    ('NRSur7dq4Remnant', 2.7654004581657494, 35.9069230085065882, [-0.0511179614344857, -0.0699047315917788, 0.1858486890134679], [-0.1626219725664848, -0.0809335994877996, 0.6786117581841464], 20.0000000000000000, 0.9632987883397930, [-0.0229309961217397, -0.0296033546567305, 0.6589413265380868], [-0.0003062881900026, 0.0004072238077448, 0.0000829064460460]),
    ('NRSur7dq4Remnant', 4.0102297532231121, 49.7518396561865544, [0.6696178784522966, -0.0089068342301592, 0.1746766925010086], [0.0279808870609607, 0.2179539532793274, 0.0167105434631247], 20.0000000000000000, 0.9723266699037116, [0.2860391720408750, 0.0184351883655322, 0.6311483360239436], [0.0015443443008142, 0.0009491236550074, 0.0027221995785075]),
    ('NRSur7dq4Remnant', 1.2697935597189383, 79.6378646107995962, [-0.0058668457790138, -0.0024491642036333, 0.0024400070716384], [-0.0058021182213425, 0.0002142173024742, 0.8303515637430949], 20.0000000000000000, 0.9421399237427907, [-0.0018927476479495, -0.0007688577219191, 0.7681048220266984], [0.0004728880522786, -0.0003377493113941, -0.0000593755964889]),
    ('NRSur7dq4Remnant', 2.6574279433433077, 77.0500174387478580, [0.0934751729618428, -0.0279017247107641, -0.4902341173316967], [0.0532365983387586, 0.2835210236215581, -0.0334119055807070], 20.0000000000000000, 0.9745447595852657, [0.0289580187120838, 0.0174283949077731, 0.3670198671072760], [0.0007263289207180, 0.0005511901897007, 0.0002363578187431]),
    ('NRSur7dq4Remnant', 4.6277065525046872, 64.3399153663480945, [0.0399217595121092, -0.0798774133248666, 0.4097113239638929], [-0.0594653514716975, -0.7279950522201541, 0.0949451611128969], 20.0000000000000000, 0.9749693160705808, [0.0207294318286621, -0.0603412244611355, 0.6665622085919323], [-0.0001448493780189, -0.0000889945542191, -0.0002028915828970]),
    ('NRSur7dq4Remnant', 5.7924060275181439, 35.2431166949661474, [-0.0026892091042226, 0.1450053306140802, -0.0195925190921984], [-0.5631877657126221, 0.2698015828114178, 0.2637400685483431], 20.0000000000000000, 0.9845730682193604, [-0.0154248641037742, 0.0622933044633797, 0.3800212324226047], [0.0002141230454137, 0.0001176278954171, -0.0006303794439186]),
    ('NRSur7dq4Remnant', 2.8614626728904899, 66.3712683141100683, [-0.0046170511941123, -0.3191230174955168, -0.7756736844154050], [0.3074814523653090, 0.4530855541984023, -0.2117912510089602], 20.0000000000000000, 0.9784862681419236, [0.0662352942227899, -0.0790495621278088, 0.2271770254230351], [0.0002481911050551, -0.0012362407199249, 0.0000835237733387]),
    ('NRSur7dq4Remnant', 3.6860643360041365, 56.1023271775251757, [-0.0681948160762584, -0.0865676308821735, -0.5052682354541183], [0.0124992554631670, 0.6673757684019793, 0.0549847801994040], 20.0000000000000000, 0.9811538135885344, [-0.0045994815073989, -0.0173053099454669, 0.2395197554217987], [-0.0003683434229595, 0.0007324953374332, -0.0003690760966181]),
    ('NRSur3dq8Remnant', 4.2170313344739592, 53.3270809535331978, [0.0000000000000000, 0.0000000000000000, -0.7974374528413865], [0.0000000000000000, 0.0000000000000000, 0.6007658219825428], -1.0000000000000000, 0.9850472325928160, [0.0000000000000000, 0.0000000000000000, 0.0278484705353544], [0.0000508858751195, -0.0008219470843442, 0.0000000000000000]),
    ('NRSur3dq8Remnant', 3.0494124096225885, 55.7123214174073951, [0.0000000000000000, 0.0000000000000000, -0.3975856741249292], [0.0000000000000000, 0.0000000000000000, -0.6961751007400377], -1.0000000000000000, 0.9776879537016973, [0.0000000000000000, 0.0000000000000000, 0.3338598416309174], [0.0003424312961141, -0.0006185610161783, 0.0000000000000000]),
    ('NRSur3dq8Remnant', 6.9093531285286902, 61.1942736441045980, [0.0000000000000000, 0.0000000000000000, -0.1776463373802170], [0.0000000000000000, 0.0000000000000000, 0.3929738054149511], -1.0000000000000000, 0.9884989489814819, [0.0000000000000000, 0.0000000000000000, 0.2216556953879781], [0.0003388641795191, -0.0002172477024889, 0.0000000000000000]),
    ('NRSur3dq8Remnant', 6.4215918478544509, 31.3646987822312049, [0.0000000000000000, 0.0000000000000000, -0.8419109303076330], [0.0000000000000000, 0.0000000000000000, -0.7092467849015318], -1.0000000000000000, 0.9910953242433421, [0.0000000000000000, 0.0000000000000000, -0.2154462078424302], [-0.0002786980424367, -0.0004150149393021, 0.0000000000000000]),
    ('NRSur3dq8Remnant', 7.2824091987145945, 37.1069551758559868, [0.0000000000000000, 0.0000000000000000, 0.6720085469644806], [0.0000000000000000, 0.0000000000000000, 0.8410432023079845], -1.0000000000000000, 0.9775533691937822, [0.0000000000000000, 0.0000000000000000, 0.7852288428156039], [-0.0000055347844263, -0.0001102981071439, 0.0000000000000000]),
    ('NRSur3dq8Remnant', 7.8823706710527732, 57.6651379216626765, [0.0000000000000000, 0.0000000000000000, 0.0583972354454951], [0.0000000000000000, 0.0000000000000000, 0.8901298792459750], -1.0000000000000000, 0.9884188333256979, [0.0000000000000000, 0.0000000000000000, 0.3562082170806193], [0.0003265001787703, 0.0000627885971843, 0.0000000000000000]),
    ('NRSur3dq8Remnant', 1.9936967398507779, 46.7533965064515726, [0.0000000000000000, 0.0000000000000000, 0.1393944612539145], [0.0000000000000000, 0.0000000000000000, -0.3936626853377779], -1.0000000000000000, 0.9606720480291526, [0.0000000000000000, 0.0000000000000000, 0.6447437032908464], [0.0003099432400108, -0.0000470835071811, 0.0000000000000000]),
    ('NRSur3dq8Remnant', 8.3471818399255557, 77.6383524576717576, [0.0000000000000000, 0.0000000000000000, 0.3721839151924385], [0.0000000000000000, 0.0000000000000000, 0.6957436526558708], -1.0000000000000000, 0.9862936938951998, [0.0000000000000000, 0.0000000000000000, 0.5669212997222990], [-0.0000907410286740, 0.0001622655878852, 0.0000000000000000]),
    ('NRSur3dq8Remnant', 1.5685041958433503, 45.2658621474996039, [0.0000000000000000, 0.0000000000000000, -0.8194877707400888], [0.0000000000000000, 0.0000000000000000, 0.2840893613475007], -1.0000000000000000, 0.9652270785849580, [0.0000000000000000, 0.0000000000000000, 0.4697314870393317], [0.0011315055274927, -0.0003721741785398, 0.0000000000000000]),
    ('NRSur3dq8Remnant', 6.9397579829551166, 50.3255966075560650, [0.0000000000000000, 0.0000000000000000, -0.1461532149846284], [0.0000000000000000, 0.0000000000000000, 0.1911625081491625], -1.0000000000000000, 0.9884187692720104, [0.0000000000000000, 0.0000000000000000, 0.2404217736056439], [0.0003268210306693, -0.0001864864936057, 0.0000000000000000]),
]

# -- test functions ---------------------

# Remove the skipif eventually, when lalsuite-extra is enabled in test pipelines
@pytest.mark.skipif("LAL_DATA_PATH" not in os.environ, reason="LAL_DATA_PATH not found.")
@pytest.mark.parametrize("model_name, q, M, chiA_vec, chiB_vec, f_ref, mf, chif_vec, vf_vec", test_data)
def test_nrfits(model_name, q, M, chiA_vec, chiB_vec, f_ref, \
        mf, chif_vec, vf_vec):
    """
    Regression test for models implemented in the lalsimulation.nrfits package.
    Add new models to test_data following the above examples.

    model_name: One of the models implemented in lalsimulation.nrfits.
    q: Mass ratio, mA/mB >= 1.
    M: Total mass in Solar masses.
    chiA_vec: Dimensionless spin vector of heavier BH.
    chiB_vec: Dimensionless spin vector of lighter BH.
    f_ref = Reference frequency used to define the frame.
    mf: Final mass, as a fraction of the total mass.
    chif_vec: Dimensionless final spin vector.
    vf_vec: Recoil kick vector in units of c.
    """

    # component masses of the binary
    m1_kg =  M*lal.MSUN_SI*q/(1.+q)
    m2_kg =  M*lal.MSUN_SI/(1.+q)

    # NOTE: Will need to be updated in the future, when PeakLuminosity fits
    # are added. Similarly, will need changes for models that only include
    # a subset of these fits.
    fit_type_list = ['FinalMass', 'FinalSpin', 'RecoilKick']

    res = eval_nrfit(m1_kg, m2_kg, chiA_vec, chiB_vec, model_name, \
            fit_type_list, f_ref=f_ref)

    mf_test = res['FinalMass']/(m1_kg + m2_kg)
    chif_vec_test = res['FinalSpin']
    vf_vec_test = res['RecoilKick']

    rtol = 1e-7
    np.testing.assert_allclose(mf, mf_test, \
        err_msg="%s FinalMass test failed."%model_name, rtol=rtol)
    np.testing.assert_allclose(chif_vec, chif_vec_test, \
        err_msg="%s FinalSpin test failed."%model_name, rtol=rtol)
    np.testing.assert_allclose(vf_vec, vf_vec_test, \
        err_msg="%s RecoilKick test failed."%model_name, rtol=rtol)


# -- run the tests ------------------------------
if __name__ == '__main__':
    sys.exit(pytest.main(args=[__file__] + sys.argv[1:] + ['-v']))
